// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class UEditorActorSubsystem;

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	const static FName LockedObjectSelectionTag;
	void SetObjectSelectionLockState(AActor* ActorToSet, bool bShouldBeLocked);
private:
#pragma region LevelMenuExtension
	void InitLevelMenuExtension();
	TSharedRef<FExtender> CreateLevelViewportMenuExtender(
		TSharedRef<FUICommandList> CommandList, 
		TArray<AActor*> InSelectedActors); 
	void AddLevelViewMenuEntry(FMenuBuilder& MenuBuilder);
	void OnLockSelectedObjectButtonClicked();
	void OnUnLockSelectedObjectButtonClicked();
#pragma endregion LevelMenuExtension
	
#pragma region ContentBrowserMenuExtension 
	TArray<FString> SelectedFolders;
	void InitCBMenuExtension();
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);
	void AddCBMenuEntry(FMenuBuilder& MenuBuilder);
	void OnDeleteUnusedAssetsButtonClicked();
	void OnDeleteEmptyFoldersButtonClicked();
	void OnAdvancedDeleteButtonClicked();
	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolder();
#pragma endregion ContentBrowserMenuExtension
	
#pragma region CustomEditorTab
	void RegisterAdvancedDeletionTab();
	static const TCHAR* AdvancedDeletionTabID;
	TSharedPtr<SDockTab> CreatedAdvancedDeletionTab; 
	TSharedRef<SDockTab> FOnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
#pragma endregion CustomEditorTab
	
	
#pragma region ObjectSelection
	void InitObjectSelection();
	void LockOrUnlockObjectSelectionEvent(UObject* SelectedObject);
	TWeakObjectPtr<UEditorActorSubsystem> EditorActorSubsystem_WeakObjectPtr;
	bool GetEditorActorSubsystem();
	static void RefreshSceneOutliner();
#pragma endregion ObjectSelection
	
#pragma region CustomUICommands
	TSharedPtr<FUICommandList> CustomUICommands;
	void InitCustomUICommands();
#pragma endregion CustomUICommands
	
#pragma region SceneOutlinerExtension
	void InitSceneOutlinerColumnExtension();
	void DeInitSceneOutlinerColumnExtension();
#pragma endregion SceneOutlinerExtension
	
#pragma region Settings
	void OnAssetCreated(const FAssetData& AssetData);
	void OnAssetRenamed(const FAssetData& AssetData, const FString& OldName);
	// 编辑器委托
	FDelegateHandle OnAssetCreatedDelegateHandle;
	FDelegateHandle OnAssetRenamedDelegateHandle;
	
	// 项目设置相关
	static const FName SettingsSectionName;
    
	// 默认命名配置
	static FString DefaultBlueprintName;
	static bool bUseCustomNaming;
	static bool bUseTimestamp;
	static bool bUseProjectPrefix;
	static FString ProjectPrefix;
	// 注册、注销 开发者设置（就是加到项目设置里面的自定义设置）
	void RegisterSettings();
	void UnRegisterSettings();
#pragma endregion Settings
};
