// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"

#include "ContentBrowserModule.h"
#include "CustomUtilities.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	InitCBMenuExtension();
}

void FSuperManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)

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
			FName("Delete"),// 这个是要附着的按钮，不是新按钮本身 （没解决）
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
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetsButtonClicked)
		);
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete empty folders")),
		FText::FromString(TEXT("Delete all empty folders under selected folders")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked)
	);
}

void FSuperManagerModule::OnDeleteUnusedAssetsButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (SelectedFolders.Num() > 1)
	{
		ShowMessageDialog(TEXT("Better select 1 folder to avoid some issues."), false);
		return;
	}
	const FString& CurrentSelectedFolder = SelectedFolders[0];
	PrintInLog(SYMBOL_NAME_TEXT(CurrentSelectedFolder)TEXT(" : ") + 
		CurrentSelectedFolder + TEXT(" in Func:") TEXT(__FUNCTION__), 
		SuperManager::ELogLevel::Display);
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
		) == EAppReturnType::No) return;
	
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
		ObjectTools::DeleteAssets(UnusedAssetsData);
	}
	else
	{
		ShowMessageDialog(TEXT("No unused assets found under the selected folder."), false);
	}
}

void FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (SelectedFolders.Num() > 1)
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
		// 忽略不为空的路径
		if (UEditorAssetLibrary::DoesDirectoryHaveAssets(AssetPath)) continue;
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

#pragma endregion ContentBrowserMenuExtention
