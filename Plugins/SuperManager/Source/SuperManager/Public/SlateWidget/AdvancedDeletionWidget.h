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
	TArray<TSharedPtr<FAssetData>> SelectedAssetsData;
	static FSlateFontInfo GetEmbossedTextFont();
	
#pragma region AssetsList
	TSharedRef<ITableRow> OnGenerateListViewRow(TSharedPtr<FAssetData> AssetData,
	                                            const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<SCheckBox> CreateAssetSelectCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	static TSharedRef<STextBlock> CreateTextBlock(const FString& TextToDisplay, const FSlateFontInfo& TextFont);
	TSharedRef<SButton> CreateDeleteOneAssetButton(const TSharedPtr<FAssetData>& AssetDataToDelete);
	void OnCheckBoxStateChanged(ECheckBoxState CheckBoxState, TSharedPtr<FAssetData> AssetData);
	TSharedRef<SListView<TSharedPtr<FAssetData>>> CreateListViewForAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToDisplay);
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetsListView;
	void UpdateAssetsListView(const TSharedPtr<FAssetData>& AssetDataDeleted);
	void UpdateAssetsListView(const TArray<TSharedPtr<FAssetData>>& AssetsDataDeleted);
#pragma endregion AssetsList
#pragma region AssetsBatchActions
	static TSharedRef<STextBlock> CreateAssetsBatchActionButtonsTextBlock(const FString& TextToDisplay);
	TSharedRef<SButton> CreateDeleteAllSelectedAssetButton(const TArray<TSharedPtr<FAssetData>>& AssetsDataToDelete);
	TSharedRef<SButton> CreateSelectAllAssetButton(const TArray<TSharedPtr<FAssetData>>& AssetsDataToDelete);
	TSharedRef<SButton> CreateDeselectAllAssetButton(const TArray<TSharedPtr<FAssetData>>& AssetsDataToDelete);
#pragma endregion AssetsBatchActions
};

