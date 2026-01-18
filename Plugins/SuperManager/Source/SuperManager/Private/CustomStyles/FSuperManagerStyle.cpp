// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomStyles/FSuperManagerStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

void FSuperManagerStyle::InitializeIcons()
{
	if (!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
	}
	FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
}

TSharedPtr<FSlateStyleSet> FSuperManagerStyle::GetCreatedSlateStyleSet()
{
	if (!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
	}
	return CreatedSlateStyleSet;
}

FName FSuperManagerStyle::StyleSetName = FName("SuperManagerStyle");
FName FSuperManagerStyle::DeleteUnusedAssetsIconName = FName("DeleteUnusedAssets");
FName FSuperManagerStyle::DeleteEmptyFoldersIconName = FName("DeleteEmptyFolders");
FName FSuperManagerStyle::AdvancedDeletionIconName = FName("AdvancedDeletion");
FName FSuperManagerStyle::SelectionLockIconName = FName("SelectionLock");
FName FSuperManagerStyle::SelectionUnlockIconName = FName("SelectionUnlock");
TSharedPtr<FSlateStyleSet> FSuperManagerStyle::CreatedSlateStyleSet = nullptr;
TSharedPtr<FSlateStyleSet> FSuperManagerStyle::CreateSlateStyleSet()
{
	constexpr auto IconFolder = TEXT("Icons");
	constexpr auto IconFileSuffix = TEXT(".png");
	TSharedPtr<FSlateStyleSet> CustomSlateStyleSet = MakeShared<FSlateStyleSet>(StyleSetName);
	const FString& PluginResourceDir =
		IPluginManager::Get().FindPlugin(TEXT("SuperManager"))->GetBaseDir() / "Resources";
	CustomSlateStyleSet->SetContentRoot(PluginResourceDir);

	const FVector2D IconSize(16.0f, 16.0f);
	auto SetIconsLambda = [&](const FName& IconName)
	{
		CustomSlateStyleSet->Set(IconName,
								 new FSlateImageBrush(
									 PluginResourceDir / IconFolder / IconName.ToString() +
									 IconFileSuffix, IconSize));
	};
	SetIconsLambda(DeleteUnusedAssetsIconName);
	SetIconsLambda(DeleteEmptyFoldersIconName);
	SetIconsLambda(AdvancedDeletionIconName);
	SetIconsLambda(SelectionLockIconName);
	SetIconsLambda(SelectionUnlockIconName);
	return CustomSlateStyleSet;
}

void FSuperManagerStyle::DeInitializeIcons()
{
	if (CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset();
	}
}
