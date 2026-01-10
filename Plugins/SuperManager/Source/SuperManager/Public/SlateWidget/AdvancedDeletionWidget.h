// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SAdvancedDeletionWidget : public SCompoundWidget
{
SLATE_BEGIN_ARGS(SAdvancedDeletionWidget){}
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, UnusedAssetsDataToStore)
SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs);
	
private:
	TArray<TSharedPtr<FAssetData>> StoredUnusedAssetsData;
	TSharedRef<ITableRow> OnGenerateListViewRow(TSharedPtr<FAssetData> AssetData,
	                                            const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<SCheckBox> CreateCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	TSharedRef<STextBlock> CreateTextBlock(const FString& TextToDisplay, const FSlateFontInfo& TextFont);
	TSharedRef<SButton> CreateDeletionButton(const TSharedPtr<FAssetData>& AssetDataToDelete);
	void OnCheckBoxStateChanged(ECheckBoxState CheckBoxState, TSharedPtr<FAssetData> AssetData);
	static FSlateFontInfo GetEmbossedTextFont();
};

