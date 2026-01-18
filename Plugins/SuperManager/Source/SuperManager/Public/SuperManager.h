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
	TSharedRef<SDockTab> FOnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
#pragma endregion CustomEditorTab
	
	
#pragma region ObjectSelection
	const FName LockedObjectSelectionTag = FName("LockedObjectSelection");
	void InitObjectSelection();
	void LockOrUnlockObjectSelectionEvent(UObject* SelectedObject);
	TWeakObjectPtr<UEditorActorSubsystem> EditorActorSubsystem;
	bool GetEditorActorSubsystem();
#pragma endregion ObjectSelection
	
#pragma region CustomUICommands
	TSharedPtr<FUICommandList> CustomUICommands;
	void InitCustomUICommands();
#pragma endregion CustomUICommands
	
#pragma region SceneOutlinerExtension
	void InitSceneOutlinerColumnExtension();
#pragma endregion SceneOutlinerExtension
};
