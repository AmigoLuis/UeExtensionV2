// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SAdvancedDeletionWidget : public SCompoundWidget
{
SLATE_BEGIN_ARGS(SAdvancedDeletionWidget){}
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AllAssetsDataToStore)
SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs);
	
private:
	TArray<TSharedPtr<FAssetData>> StoredAllAssetsData;
	TArray<TSharedPtr<FAssetData>> DisplayedAssetsData;
	TArray<TSharedPtr<FAssetData>> DisplayedSelectedAssetsData;
	TArray<TSharedPtr<FAssetData>> UnDisplayedSelectedAssetsData;
	TMap<TSharedPtr<FAssetData>, TSharedPtr<SCheckBox>> AssetsAndCheckBoxes;
	static FSlateFontInfo GetEmbossedTextFont();
	
#pragma region AssetListConditionComboBoxes
	TSharedRef<SComboBox<TSharedPtr<FString>>> CreateAssetListConditionComboBox();
	TArray<TSharedPtr<FString>> AssetListConditionStrings;
	TSharedPtr<STextBlock> ComboDisplayTextBlock;
	void OnListConditionSelectionChanged(TSharedPtr<FString> SelectedListCondition, ESelectInfo::Type InSelectionInfo);
	TSharedRef<SWidget> OnGenerateAssetListCondition(TSharedPtr<FString> ListConditionString);
	void UpdateAssetsListViewWhenConditionChanged();
#pragma endregion AssetListConditionComboBoxes
#pragma region AssetsList
	TSharedRef<SListView<TSharedPtr<FAssetData>>> CreateListViewForAssets(const TArray<TSharedPtr<FAssetData>>& AssetDataToDisplay);
	void OnMouseClickListView(TSharedPtr<FAssetData> ClickedListItem);
	TSharedRef<ITableRow> OnGenerateListViewRow(TSharedPtr<FAssetData> AssetData,
	                                            const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<SCheckBox> CreateAssetSelectCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	static TSharedRef<STextBlock> CreateTextBlock(const FString& TextToDisplay, const FSlateFontInfo& TextFont);
	TSharedRef<SButton> CreateDeleteOneAssetButton(const TSharedPtr<FAssetData>& AssetDataToDelete);
	void OnCheckBoxStateChanged(ECheckBoxState CheckBoxState, TSharedPtr<FAssetData> AssetData);
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetsListView;
	void UpdateAssetsListViewWhenDeletedAssets(const TSharedPtr<FAssetData>& AssetDataDeleted);
	void UpdateAssetsListViewWhenDeletedAssets(const TArray<TSharedPtr<FAssetData>>& AssetsDataDeleted);
#pragma endregion AssetsList
#pragma region AssetsBatchActions
	TSharedRef<SHorizontalBox> CreateHorizontalBoxAndBatchAssetActionButtons();
	static TSharedRef<STextBlock> CreateAssetsBatchActionButtonsTextBlock(const FString& TextToDisplay);
	TSharedRef<SButton> CreateDeleteAllSelectedAssetButton();
	FReply OnDeleteAllSelectedAssetsButtonClicked();
	TSharedRef<SButton> CreateSelectAllAssetButton();
	FReply OnSelectAllAssetButtonClicked();
	TSharedRef<SButton> CreateDeselectAllAssetButton();
	FReply OnDeselectAllAssetButtonClicked();
	TSharedRef<SButton> CreateToggleAllAssetButton();
	FReply OnToggleAllAssetButtonClicked();
#pragma endregion AssetsBatchActions
};

