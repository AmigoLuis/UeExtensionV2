// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
#pragma region LevelMenuExtention
	void InitLevelMenuExtension();
	TSharedRef<FExtender> CreateLevelViewportMenuExtender(
		TSharedRef<FUICommandList> CommandList, 
		TArray<AActor*> InSelectedActors); 
	void AddLevelViewMenuEntry(FMenuBuilder& MenuBuilder);
	void OnLockSelectedObjectButtonClicked();
	void OnUnLockSelectedObjectButtonClicked();
	TArray<AActor*> SelectedActors;
#pragma endregion LevelMenuExtention
	
#pragma region ContentBrowserMenuExtention 
	TArray<FString> SelectedFolders;
	void InitCBMenuExtension();
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);
	void AddCBMenuEntry(FMenuBuilder& MenuBuilder);
	void OnDeleteUnusedAssetsButtonClicked();
	void OnDeleteEmptyFoldersButtonClicked();
	void OnAdvancedDeleteButtonClicked();
	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolder();
#pragma endregion ContentBrowserMenuExtention
	
#pragma region CustomEditorTab
	void RegisterAdvancedDeletionTab();
	static const TCHAR* AdvancedDeletionTabID;
	TSharedRef<SDockTab> FOnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
#pragma endregion CustomEditorTab
	
	
#pragma region ObjectSelection
	void InitObjectSelection();
	void LockOrUnlockObjectSelection(UObject* SelectedObject);
#pragma endregion ObjectSelection
};
