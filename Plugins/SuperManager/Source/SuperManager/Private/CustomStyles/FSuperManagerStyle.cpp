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
FName FSuperManagerStyle::SelectionLockToggleButtonIconName = FName("SelectionLockToggleButton");
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
	
	const FString& InImageName = PluginResourceDir / IconFolder / SelectionLockIconName.ToString() + IconFileSuffix;
	const FCheckBoxStyle& CheckBoxStyle = FCheckBoxStyle()
	.SetCheckBoxType(ESlateCheckBoxType::Type::ToggleButton).SetPadding(FMargin(10.0f))
	// unchecked
	.SetUncheckedImage(FSlateImageBrush(InImageName, IconSize, FStyleColors::AccentGreen))
	.SetUncheckedHoveredImage(FSlateImageBrush(InImageName, IconSize, FStyleColors::White25))
	.SetUncheckedPressedImage(FSlateImageBrush(InImageName, IconSize, FStyleColors::AccentYellow))
	// checked
	.SetCheckedImage(FSlateImageBrush(InImageName, IconSize, FStyleColors::AccentGray))
	.SetCheckedHoveredImage(FSlateImageBrush(InImageName, IconSize, FStyleColors::AccentWhite))
	.SetCheckedPressedImage(FSlateImageBrush(InImageName, IconSize, FStyleColors::Foreground));
	CustomSlateStyleSet->Set(SelectionLockToggleButtonIconName,CheckBoxStyle);
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
