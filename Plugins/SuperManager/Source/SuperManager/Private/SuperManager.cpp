// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "AssetToolsModule.h"
#include "CheckAndLogAndReturn.h"
#include "ContentBrowserModule.h"
#include "CustomUtilities.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "SceneOutlinerModule.h"
#include "Selection.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "CustomOutliner/FOutlinerSelectionLock.h"
#include "CustomStyles/FSuperManagerStyle.h"
#include "SlateWidget/AdvancedDeletionWidget.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "CustomUICommands/SuperManagerUICommands.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "SuperManager/AssetActions/BlueprintAssetHandlers/FBlueprintAssetRenameHandler.h"
#include "SuperManager/AssetActions/Settings/BPDefaultNameSettings.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FSuperManagerStyle::InitializeIcons();
	FSuperManagerUICommands::Register();
	InitCustomUICommands();
	InitLevelMenuExtension();
	InitCBMenuExtension();
	InitObjectSelection();
	RegisterAdvancedDeletionTab();
	InitSceneOutlinerColumnExtension();
	
#pragma region RegisterSettingsForModifyDefaultBlueprintName
	RegisterSettings();
 
	// 注册委托

	const FAssetRegistryModule* AssetRegistryModulePtr = LoadModulePtrWithLog<FAssetRegistryModule>("AssetRegistry");
	CHECK_NULL_RETURN(AssetRegistryModulePtr);
	AssetRegistryModulePtr->Get().OnFilesLoaded().AddLambda([this, AssetRegistryModulePtr]()
	{
		// 仅在初始扫描完成后再监听新增资产事件
		OnAssetCreatedDelegateHandle = AssetRegistryModulePtr->Get().OnAssetAdded().AddRaw(
			this, &FSuperManagerModule::OnAssetCreated);
		OnAssetRenamedDelegateHandle = AssetRegistryModulePtr->Get().OnAssetRenamed().AddRaw(
			this, &FSuperManagerModule::OnAssetRenamed);
	});
    
#pragma endregion RegisterSettingsForModifyDefaultBlueprintName
}

void FSuperManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(AdvancedDeletionTabID);
	FSuperManagerStyle::DeInitializeIcons();
	FSuperManagerUICommands::Unregister();
	DeInitSceneOutlinerColumnExtension();
	{
		// 取消注册设置
		UnRegisterSettings();
		// 取消注册委托
		if (FModuleManager::Get().IsModuleLoaded("AssetRegistry"))
		{
			const FAssetRegistryModule* AssetRegistryModulePtr = LoadModulePtrWithLog<FAssetRegistryModule>("AssetRegistry");
			CHECK_NULL_RETURN(AssetRegistryModulePtr);
			AssetRegistryModulePtr->Get().OnAssetAdded().Remove(OnAssetCreatedDelegateHandle);
			AssetRegistryModulePtr->Get().OnAssetAdded().Remove(OnAssetRenamedDelegateHandle);
		}
    
	}
}


IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)

#pragma region LevelMenuExtention
void FSuperManagerModule::InitLevelMenuExtension()
{
	using FSelectedActors = FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors;
	FLevelEditorModule* LevelEditorModulePtr = LoadModulePtrWithLog<
		FLevelEditorModule>(TEXT("LevelEditor"));
	if (!LevelEditorModulePtr) return;

	const TSharedRef<FUICommandList>& GlobalLevelEditorActions = 
		LevelEditorModulePtr->GetGlobalLevelEditorActions();
	GlobalLevelEditorActions->Append(CustomUICommands.ToSharedRef());
	
	TArray<FSelectedActors>& LevelViewPortMenuExtenders = LevelEditorModulePtr->
		GetAllLevelViewportContextMenuExtenders();
	LevelViewPortMenuExtenders.Add(
		FSelectedActors::CreateRaw(this, &FSuperManagerModule::CreateLevelViewportMenuExtender));
	
}

TSharedRef<FExtender> FSuperManagerModule::CreateLevelViewportMenuExtender(TSharedRef<FUICommandList> CommandList,
	TArray<AActor*> InSelectedActors)
{
	TSharedRef<FExtender> Extender = MakeShareable(new FExtender);
	if (InSelectedActors.Num() > 0)
	{
		Extender->AddMenuExtension(
			FName("SaveActor"),
			EExtensionHook::Before,
			CommandList,
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddLevelViewMenuEntry)
		);
	}
	return Extender;
}

void FSuperManagerModule::AddLevelViewMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Lock selected objects")),
		FText::FromString(TEXT("Lock all selected objects so that you can't select them until you unlock them.")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), 
			FSuperManagerStyle::GetSelectionLockIconName()),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnLockSelectedObjectButtonClicked)
		);
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Unlock all objects")),
		FText::FromString(TEXT("Unlock all selected objects so that you can select them again.")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), 
			FSuperManagerStyle::GetSelectionUnlockIconName()),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnUnLockSelectedObjectButtonClicked)
	);
}

void FSuperManagerModule::OnLockSelectedObjectButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (!GetEditorActorSubsystem()) return;
	uint8 NumOfLockedObjects = 0;
	for (TArray<AActor*> SelectedActors = EditorActorSubsystem_WeakObjectPtr->GetSelectedLevelActors(); 
		AActor* SelectedActor : SelectedActors)
	{
		if (SelectedActor == nullptr) continue;
		if (!SelectedActor->ActorHasTag(LockedObjectSelectionTag))
		{
			SelectedActor->Tags.Add(LockedObjectSelectionTag);
			EditorActorSubsystem_WeakObjectPtr->SetActorSelectionState(SelectedActor, false);
			++NumOfLockedObjects;
		}
	}
	if (NumOfLockedObjects > 0)
	{
		ShowNotifyInfo(FString::Format(TEXT("Locked {0} objects' selection."), 
			{NumOfLockedObjects}));
	}
	else
	{
		ShowMessageDialog(TEXT("Not lock any object's selection."));
	}
	RefreshSceneOutliner();
}

void FSuperManagerModule::OnUnLockSelectedObjectButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (!GetEditorActorSubsystem()) return;
	uint8 NumOfUnlockedObjects = 0;
	for (const TArray<AActor*>& AllLevelActors = EditorActorSubsystem_WeakObjectPtr->GetAllLevelActors(); 
		AActor* SelectedActor : AllLevelActors)
	{
		if (SelectedActor == nullptr) continue;
		if (SelectedActor->ActorHasTag(LockedObjectSelectionTag))
		{
			SelectedActor->Tags.Remove(LockedObjectSelectionTag);
			++NumOfUnlockedObjects;
		}
	}
	if (NumOfUnlockedObjects > 0)
	{
		ShowNotifyInfo(FString::Format(TEXT("Unlocked {0} objects' selection."), 
			{NumOfUnlockedObjects}));
	}
	else
	{
		ShowMessageDialog(TEXT("Not unlock any object's selection."));
	}
	RefreshSceneOutliner();
}
#pragma endregion LevelMenuExtention

#pragma region ContentBrowserMenuExtention
void FSuperManagerModule::InitCBMenuExtension()
{
	using FSelectedPaths = FContentBrowserMenuExtender_SelectedPaths;
	FContentBrowserModule* ContentBrowserModulePtr = LoadModulePtrWithLog<
		FContentBrowserModule>(TEXT("ContentBrowser"));
	if (!ContentBrowserModulePtr) return;
	TArray<FSelectedPaths>& CBMenuExtenders = ContentBrowserModulePtr->
		GetAllPathViewContextMenuExtenders();
	CBMenuExtenders.Add(FSelectedPaths::CreateRaw(this, &FSuperManagerModule::CustomCBMenuExtender));
}

TSharedRef<FExtender> FSuperManagerModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());
	SelectedFolders = SelectedPaths;
	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension(
			FName("Delete"), // 这个是要附着的按钮，不是新按钮本身 （没解决）
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddCBMenuEntry)
		);
	}
	return MenuExtender;
}

void FSuperManagerModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete unused assets")),
		FText::FromString(TEXT("Delete all unused assets under selected folder")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), 
			FSuperManagerStyle::GetDeleteUnusedAssetsIconName()),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetsButtonClicked)
	);
	
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete empty folders")),
		FText::FromString(TEXT("Delete all empty folders under selected folder")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), 
			FSuperManagerStyle::GetDeleteEmptyFoldersIconName()),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked)
	);
	
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Advanced delete")),
		FText::FromString(TEXT("Advanced delete empty folders and unused assets under selected folder")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), 
			FSuperManagerStyle::GetAdvancedDeletionIconName()),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnAdvancedDeleteButtonClicked)
	);
}

void FSuperManagerModule::OnDeleteUnusedAssetsButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (CreatedAdvancedDeletionTab.IsValid())
	{
		ShowMessageDialog(TEXT("Operation canceled cause 'Advanced Deletion Tab' is still open."));
		return;
	}
	if (SelectedFolders.Num() != 1)
	{
		ShowMessageDialog(TEXT("Better select 1 folder to avoid some issues."), false);
		return;
	}
	const FString& CurrentSelectedFolder = SelectedFolders[0];
	PrintInLog(SYMBOL_NAME_TEXT(CurrentSelectedFolder)TEXT(" : ") +
	           CurrentSelectedFolder + TEXT(" in Func:") TEXT(__FUNCTION__),
	           Display);
	TArray<FString> AssetPaths = UEditorAssetLibrary::ListAssets(CurrentSelectedFolder);
	if (AssetPaths.IsEmpty())
	{
		ShowMessageDialog(TEXT("No assets found under the selected folder:")
		                  + CurrentSelectedFolder + TEXT("."), false);
		return;
	}

	if (ShowMessageDialog(
		FString::FromInt(AssetPaths.Num()) +
		TEXT(" assets found under the selected folder:") + CurrentSelectedFolder +
		TEXT(".\n Still proceed to delete unused assets among them?")
		, true, EAppMsgType::YesNo
	) == EAppReturnType::No)
		return;

	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetsData;
	for (const FString& AssetPath : AssetPaths)
	{
		// 忽略ue内部的路径
		if (IsUnrealProtectedPath(AssetPath)) continue;
		// 忽略不存在的路径
		if (!UEditorAssetLibrary::DoesAssetExist(AssetPath)) continue;
		// 忽略存在包引用的资产
		if (!UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPath).IsEmpty()) continue;

		UnusedAssetsData.Add(UEditorAssetLibrary::FindAssetData(AssetPath));
	}

	if (!UnusedAssetsData.IsEmpty())
	{
		DeleteAssetsAndLog(UnusedAssetsData);
	}
	else
	{
		ShowMessageDialog(TEXT("No unused assets found under the selected folder."), false);
	}
}

void FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (CreatedAdvancedDeletionTab.IsValid())
	{
		ShowMessageDialog(TEXT("Operation canceled cause 'Advanced Deletion Tab' is still open."));
		return;
	}
	if (SelectedFolders.Num() != 1)
	{
		ShowMessageDialog(TEXT("Better select 1 folder to avoid some issues."), false);
		return;
	}
	FixUpRedirectors();
	FString EmptyFolderNames;
	TArray<FString> EmptyFolders;
	const FString& SelectedFolder = SelectedFolders[0];
	TArray<FString> AssetPaths = UEditorAssetLibrary::ListAssets(SelectedFolder, true, true);
	if (AssetPaths.IsEmpty())
	{
		ShowMessageDialog(TEXT("Nothing found under the selected folder:")
		                  + SelectedFolder + TEXT("."), false);
		return;
	}
	for (const FString& AssetPath : AssetPaths)
	{
		// 忽略ue内部的路径
		if (IsUnrealProtectedPath(AssetPath)) continue;
		// 忽略不存在的路径
		if (!UEditorAssetLibrary::DoesDirectoryExist(AssetPath)) continue;
		// UEditorAssetLibrary::DoesDirectoryHaveAssets的入参路径如果以"/"结尾，即使目录下有资产也返回false，所以必须去除结尾的"/"
		FString AssetPathWithNoEndSlash = AssetPath;
		if (AssetPathWithNoEndSlash.EndsWith("/"))
		{
			AssetPathWithNoEndSlash.RemoveAt(AssetPathWithNoEndSlash.Len() - 1);
		}
		// 忽略不为空的路径
		if (UEditorAssetLibrary::DoesDirectoryHaveAssets(AssetPathWithNoEndSlash)) continue;
		EmptyFolderNames.Append(AssetPath);
		EmptyFolderNames.Append(TEXT("\n"));
		EmptyFolders.Add(AssetPath);
	}


	if (EmptyFolderNames.IsEmpty())
	{
		ShowMessageDialog(TEXT("No empty folders found under the selected folder:")
		                  + SelectedFolder + TEXT("."), false);
		return;
	}

	const EAppReturnType::Type UserChoice =
		ShowMessageDialog(
			FString::FromInt(EmptyFolders.Num()) + TEXT(
				" empty folders found(listed below).\nwould you like to delete them all?\n") + EmptyFolderNames, true,
			EAppMsgType::OkCancel);
	if (UserChoice == EAppReturnType::Cancel) return;
	uint32 DeletedFoldersCounter = 0;
	for (const FString& EmptyFolder : EmptyFolders)
	{
		if (!UEditorAssetLibrary::DeleteDirectory(EmptyFolder))
		{
			ShowNotifyInfo(TEXT("directory:") + EmptyFolder + TEXT(" could not be deleted."));
		}
		else
		{
			++DeletedFoldersCounter;
		}
	}
	if (DeletedFoldersCounter > 0)
	{
		ShowMessageDialog(FString::FromInt(DeletedFoldersCounter) + TEXT(" empty folders have been deleted."), false);
	}
}

void FSuperManagerModule::OnAdvancedDeleteButtonClicked()
{
	// AdvanceDelete
	LOG_ENTER_FUNCTION();
	FGlobalTabmanager::Get()->TryInvokeTab(FTabId(AdvancedDeletionTabID));
}

TArray<TSharedPtr<FAssetData>> FSuperManagerModule::GetAllAssetDataUnderSelectedFolder()
{
	TArray<TSharedPtr<FAssetData>> AllAssetsData;
	if (SelectedFolders.Num() != 1)
	{
		ShowMessageDialog(TEXT("Better select 1 folder to avoid some issues."), false);
		return AllAssetsData;
	}
	const FString& CurrentSelectedFolder = SelectedFolders[0];
	PrintInLog(SYMBOL_NAME_TEXT(CurrentSelectedFolder)TEXT(" : ") +
			   CurrentSelectedFolder + TEXT(" in Func:") TEXT(__FUNCTION__),
			   Display);
	TArray<FString> AssetPaths = UEditorAssetLibrary::ListAssets(CurrentSelectedFolder);
	if (AssetPaths.IsEmpty())
	{
		ShowMessageDialog(TEXT("No assets found under the selected folder:")
						  + CurrentSelectedFolder + TEXT("."), false);
		return AllAssetsData;
	}
	FixUpRedirectors();

	for (const FString& AssetPath : AssetPaths)
	{
		// 忽略ue内部的路径
		if (IsUnrealProtectedPath(AssetPath)) continue;
		// 忽略不存在的路径
		if (!UEditorAssetLibrary::DoesAssetExist(AssetPath)) continue;

		AllAssetsData.Emplace(MakeShared<FAssetData>(UEditorAssetLibrary::FindAssetData(AssetPath)));
	}
	
	if (AllAssetsData.IsEmpty())
	{
		ShowMessageDialog(TEXT("No unused assets found under the selected folder.")
						  + CurrentSelectedFolder + TEXT("."), false);
	}
	return AllAssetsData;
}

#pragma endregion ContentBrowserMenuExtention

#pragma region CustomEditorTab
const TCHAR* FSuperManagerModule::AdvancedDeletionTabID = TEXT("Advanced Deletion");

void FSuperManagerModule::RegisterAdvancedDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(AdvancedDeletionTabID,
	                                                  FOnSpawnTab::CreateRaw(
		                                                  this,
		                                                  &FSuperManagerModule::FOnSpawnAdvancedDeletionTab))
	                        .SetDisplayName(FText::FromString(AdvancedDeletionTabID))
	                        .SetIcon(FSlateIcon(FSuperManagerStyle::GetStyleSetName(),
	                                            FSuperManagerStyle::GetAdvancedDeletionIconName()));
}

TSharedRef<SDockTab> FSuperManagerModule::FOnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	if (SelectedFolders.IsEmpty()) return SNew(SDockTab).TabRole(NomadTab);
	CreatedAdvancedDeletionTab = SNew(SDockTab).TabRole(NomadTab)
		[
			SNew(SAdvancedDeletionWidget)
			.AllAssetsDataToStore(GetAllAssetDataUnderSelectedFolder())
			.CurrentFolderPath(MakeShared<FString>(SelectedFolders[0]))
		];
	
	CreatedAdvancedDeletionTab->SetOnTabClosed(
		SDockTab::FOnTabClosedCallback::CreateLambda(
			[this](TSharedRef<SDockTab> ClosingTab)
			{
				this->CreatedAdvancedDeletionTab.Reset();
				this->SelectedFolders.Empty();
			}));
	return CreatedAdvancedDeletionTab.ToSharedRef();
}
#pragma endregion CustomEditorTab

#pragma region ObjectSelection
const FName FSuperManagerModule::LockedObjectSelectionTag = FName("LockedObjectSelection");

void FSuperManagerModule::SetObjectSelectionLockState(AActor* ActorToSet, const bool bShouldBeLocked)
{
	if (!ActorToSet || !GetEditorActorSubsystem()) return;
	if (bShouldBeLocked)
	{
		if (!ActorToSet->ActorHasTag(LockedObjectSelectionTag))
		{
			ActorToSet->Tags.Add(LockedObjectSelectionTag);
			EditorActorSubsystem_WeakObjectPtr->SetActorSelectionState(ActorToSet, false);
			ShowNotifyInfo(FString::Format(
				TEXT("Actor: {0}'s selection is locked"), 
				{ActorToSet->GetActorLabel()}));
		}
	}
	else
	{
		if (ActorToSet->ActorHasTag(LockedObjectSelectionTag))
		{
			ActorToSet->Tags.Remove(LockedObjectSelectionTag);
			ShowNotifyInfo(FString::Format(
				TEXT("Actor: {0}'s selection is unlocked"), 
				{ActorToSet->GetActorLabel()}));
		}
	}
}

void FSuperManagerModule::InitObjectSelection()
{
	CHECK_NULL_RETURN(GEditor);
	USelection* UserSelections = GEditor->GetSelectedActors();
	UserSelections->SelectObjectEvent.AddRaw(this, &FSuperManagerModule::LockOrUnlockObjectSelectionEvent);
}

void FSuperManagerModule::LockOrUnlockObjectSelectionEvent(UObject* SelectedObject)
{
	if (AActor* InSelectedActor = Cast<AActor>(SelectedObject); InSelectedActor != nullptr && GetEditorActorSubsystem())
	{
		if (InSelectedActor->ActorHasTag(LockedObjectSelectionTag))
		{
			EditorActorSubsystem_WeakObjectPtr->SetActorSelectionState(InSelectedActor, false);
			PrintInLog(FString::Format(
				TEXT("Actor: {0} can not be selected in level cause its selection is locked"), 
				{InSelectedActor->GetActorLabel()}));
		}
	}
	else if (SelectedObject != nullptr)
	{
		PrintInLog(SelectedObject->GetName() + TEXT(" (GetName) Cast to AActor failed."));
		PrintInLog(SelectedObject->GetPathName() + TEXT(" (GetPathName) Cast to AActor failed."));
		PrintInLog(SelectedObject->GetFName().ToString() + TEXT(" (GetFName) Cast to AActor failed."));
	}
	else
	{
		PrintInLog(TEXT("selected object is nullptr."));
	}
}
bool FSuperManagerModule::GetEditorActorSubsystem()
{
	if (!EditorActorSubsystem_WeakObjectPtr.IsValid())
	{
		EditorActorSubsystem_WeakObjectPtr = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}
	return EditorActorSubsystem_WeakObjectPtr.IsValid();
}

void FSuperManagerModule::RefreshSceneOutliner()
{
	const FLevelEditorModule* LevelEditorModulePtr = LoadModulePtrWithLog<
		FLevelEditorModule>(TEXT("LevelEditor"));
	if (!LevelEditorModulePtr) return;
	TArray<TWeakPtr<ISceneOutliner>> SceneOutliners = LevelEditorModulePtr->GetFirstLevelEditor()->
	                                                                        GetAllSceneOutliners();
	for (TWeakPtr SceneOutliner : SceneOutliners)
	{
		if (SceneOutliner.IsValid())
		{
			SceneOutliner.Pin()->FullRefresh();
		}
	}
}
#pragma endregion ObjectSelection

#pragma region CustomUICommands
void FSuperManagerModule::InitCustomUICommands()
{
	CustomUICommands = MakeShareable(new FUICommandList);
	CustomUICommands->MapAction(FSuperManagerUICommands::Get().LockActorSelectionCommandInfo,
	                            FExecuteAction::CreateRaw(
		                            this, &FSuperManagerModule::OnLockSelectedObjectButtonClicked));
	CustomUICommands->MapAction(FSuperManagerUICommands::Get().UnlockActorSelectionCommandInfo,
	                            FExecuteAction::CreateRaw(
		                            this, &FSuperManagerModule::OnUnLockSelectedObjectButtonClicked));
}
#pragma endregion CustomUICommands

#pragma region SceneOutlinerExtension
#define SceneOutlinerModuleName TEXT("SceneOutliner")
void FSuperManagerModule::InitSceneOutlinerColumnExtension()
{
	FSceneOutlinerModule* SceneOutlinerModulePtr = LoadModulePtrWithLog<FSceneOutlinerModule>(SceneOutlinerModuleName);
	if (!SceneOutlinerModulePtr) return;
	const FSceneOutlinerColumnInfo SelectionLockColumnInfo(ESceneOutlinerColumnVisibility::Visible, 0,
	                                                       FCreateSceneOutlinerColumn::CreateLambda(
		                                                       [](ISceneOutliner& SceneOutliner)
		                                                       {
			                                                       return MakeShareable(
				                                                       new FOutlinerSelectionLock(SceneOutliner));
		                                                       }));
	SceneOutlinerModulePtr->RegisterDefaultColumnType<FOutlinerSelectionLock>(SelectionLockColumnInfo);
}

void FSuperManagerModule::DeInitSceneOutlinerColumnExtension()
{
	FSceneOutlinerModule* SceneOutlinerModulePtr = LoadModulePtrWithLog<FSceneOutlinerModule>(SceneOutlinerModuleName);
	if (!SceneOutlinerModulePtr) return;
	SceneOutlinerModulePtr->UnRegisterColumnType<FOutlinerSelectionLock>();
}
#pragma endregion SceneOutlinerExtension

#pragma region Settings

void FSuperManagerModule::OnAssetCreated(const FAssetData& AssetData)
{	
	// 使用异步方式处理，避免在主线程中同步加载
	Async(EAsyncExecution::TaskGraphMainThread, [AssetData]()
	{
		FBlueprintAssetRenameHandler::ProcessAssetIfIsBlueprint(AssetData);
	});
	
}

void FSuperManagerModule::OnAssetRenamed(const FAssetData& AssetData, const FString& OldName)
{
	PrintInLog(TEXT("AssetRenamed: ") + AssetData.GetFullName() + TEXT(", Asset old name:") + OldName);
}

void FSuperManagerModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", 
			"Blueprint Default Name",
			LOCTEXT("RuntimeSettingsName", "Blueprint Default Name"),
			LOCTEXT("RuntimeSettingsDescription", "Configure default naming for Blueprints"),
			GetMutableDefault<UBPDefaultNameSettings>()
		);
	}
}

void FSuperManagerModule::UnRegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", 
			"Plugins", "Blueprint Default Name");
	}
}
#pragma endregion Settings

#undef LOCTEXT_NAMESPACE
