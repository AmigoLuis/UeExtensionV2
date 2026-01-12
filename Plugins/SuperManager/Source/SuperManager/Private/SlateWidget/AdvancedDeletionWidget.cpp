// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidget/AdvancedDeletionWidget.h"

#include "CustomUtilities.h"
#include "DebugHeader.h"

#define CONDITION_LIST_ALL_TEXT TEXT("List All Assets")

void SAdvancedDeletionWidget::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
	FSlateFontInfo TitleTextFont = GetEmbossedTextFont();
	TitleTextFont.Size = 30;

	StoredUnusedAssetsData = InArgs._UnusedAssetsDataToStore;
	if (StoredUnusedAssetsData.Num() == 0) return;
	SelectedAssetsData.Empty();
	CheckBoxesOfAssets.Empty();
	
	AssetListConditionStrings.Add(MakeShared<FString>(CONDITION_LIST_ALL_TEXT));
	
	ChildSlot[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight() 
		[	// 标题slot
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Advance Deletion")))
			.Font(TitleTextFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]
		
		+ SVerticalBox::Slot().AutoHeight()
		[	// 资产列表条件，决定哪些资产会出现在列表中
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
			[
				CreateAssetListConditionComboBox()
			]
		]
		
		+ SVerticalBox::Slot().VAlign(VAlign_Fill) 
		[	// 资产展示列表
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				CreateListViewForAssets(StoredUnusedAssetsData)
			]
		]
		
		+ SVerticalBox::Slot().AutoHeight()
		[	// 资产批量操作按钮
			CreateHorizontalBoxAndBatchAssetActionButtons()
		]
	];
}

TSharedRef<STextBlock> SAdvancedDeletionWidget::CreateTextBlock(const FString& TextToDisplay,
                                                                const FSlateFontInfo& TextFont)
{
	return SNew(STextBlock)
		.Text(FText::FromString(TextToDisplay))
		.Font(TextFont)
		// .Justification(ETextJustify::Center)
		.ColorAndOpacity(FColor::White);
}

FSlateFontInfo SAdvancedDeletionWidget::GetEmbossedTextFont()
{
	return FCoreStyle::Get().GetFontStyle(TEXT("EmbossedText"));
}

#pragma region AssetListConditionComboBoxes
TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvancedDeletionWidget::CreateAssetListConditionComboBox()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> AssetListConditionComboBox = 
		SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&AssetListConditionStrings)
			.OnGenerateWidget(this, &SAdvancedDeletionWidget::OnGenerateAssetListCondition)
		.OnSelectionChanged(this, &SAdvancedDeletionWidget::OnListConditionSelectionChanged)
		[
			SAssignNew(ComboDisplayTextBlock, STextBlock).Text(FText::FromString(TEXT("List Assets Options")))
		];
	return AssetListConditionComboBox;
}

void SAdvancedDeletionWidget::OnListConditionSelectionChanged(TSharedPtr<FString> SelectedListCondition,
	ESelectInfo::Type InSelectionInfo)
{
	PrintInLog(*SelectedListCondition, SuperManager::ELogLevel::Display);
	ComboDisplayTextBlock->SetText(FText::FromString(*SelectedListCondition));
}

TSharedRef<SWidget> SAdvancedDeletionWidget::OnGenerateAssetListCondition(
	TSharedPtr<FString> ListConditionString)
{
	return SNew(STextBlock).Text(FText::FromString(*ListConditionString));
}

#pragma endregion AssetListConditionComboBoxes

#pragma region AssetsList
TSharedRef<ITableRow> SAdvancedDeletionWidget::OnGenerateListViewRow(TSharedPtr<FAssetData> AssetData,
                                                                     const TSharedRef<STableViewBase>& OwnerTable)
{
	FSlateFontInfo AssetClassTextFont = GetEmbossedTextFont();
	AssetClassTextFont.Size = 10;
	return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(3.0f))
		[
			SNew(SHorizontalBox)
			// 1 select asset checkbox
			+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Center).FillWidth(0.05f)
			[
				CreateAssetSelectCheckBox(AssetData)
			]
			// 2 asset class name
			+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Fill).FillWidth(0.4f)
			[
				CreateTextBlock(AssetData->AssetClassPath.GetAssetName().ToString(), AssetClassTextFont)
			]
			// 3 asset name
			+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Fill)
			[
				CreateTextBlock(AssetData->AssetName.ToString(), AssetClassTextFont)
			]
			// 4 delete button
			+ SHorizontalBox::Slot().HAlign(HAlign_Right).VAlign(VAlign_Fill)//.FillWidth(0.2f)
			[
				CreateDeleteOneAssetButton(AssetData)
			]
		];
}

TSharedRef<SCheckBox> SAdvancedDeletionWidget::CreateAssetSelectCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> CreatedCheckBox = SNew(SCheckBox)
		.Type(ESlateCheckBoxType::Type::CheckBox)
		.OnCheckStateChanged(this, &SAdvancedDeletionWidget::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);
	CheckBoxesOfAssets.Emplace(AssetDataToDisplay,CreatedCheckBox.ToSharedPtr());
	return CreatedCheckBox;
}

TSharedRef<SButton> SAdvancedDeletionWidget::CreateDeleteOneAssetButton(const TSharedPtr<FAssetData>& AssetDataToDelete)
{
	TSharedRef<SButton> DeletionButton = SNew(SButton).Text(FText::FromString(TEXT("DeleteAsset"))).OnClicked_Lambda(
			[AssetDataToDelete, this]
			{
				if (DeleteAssetsAndLog(*AssetDataToDelete) > 0)
				{
					UpdateAssetsListView(AssetDataToDelete);
				}
				return FReply::Handled();
			});
	return DeletionButton;
}

void SAdvancedDeletionWidget::OnCheckBoxStateChanged(ECheckBoxState CheckBoxState, TSharedPtr<FAssetData> AssetData)
{
	switch (CheckBoxState)
	{
		case ECheckBoxState::Checked:
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is checked."), SuperManager::Display);
		SelectedAssetsData.AddUnique(AssetData);
		break;
	case ECheckBoxState::Unchecked:
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is unchecked."), SuperManager::Display);
		SelectedAssetsData.Remove(AssetData);
		break;
	case ECheckBoxState::Undetermined:
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is undetermined."), SuperManager::Display);
		break;
	default:
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is unknown."), SuperManager::Display);
		break;
	}
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvancedDeletionWidget::CreateListViewForAssets(
	const TArray<TSharedPtr<FAssetData>>& AssetDataToDisplay)
{
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ListView =
		SNew(SListView<TSharedPtr<FAssetData>>)
		.ListItemsSource(&AssetDataToDisplay)
		.OnGenerateRow(this, &SAdvancedDeletionWidget::OnGenerateListViewRow);
	AssetsListView = ListView.ToSharedPtr();
	return ListView;
}

void SAdvancedDeletionWidget::UpdateAssetsListView(const TSharedPtr<FAssetData>& AssetDataDeleted)
{
	// clear deleted asset's checkbox reference before SelectedAssetsData.Remove
	// because AssetsDataDeleted might be in itself (or StoredUnusedAssetsData)
	// if u call CheckBoxesOfAssets.Remove after SelectedAssetsData.RemoveAll
	// u won't be able to find any data to delete cause its already deleted
	CheckBoxesOfAssets.Remove(AssetDataDeleted);
	StoredUnusedAssetsData.Remove(AssetDataDeleted);
	SelectedAssetsData.Remove(AssetDataDeleted);
	if (AssetsListView.IsValid())
	{
		AssetsListView->RequestListRefresh();
	}
}

void SAdvancedDeletionWidget::UpdateAssetsListView(const TArray<TSharedPtr<FAssetData>>& AssetsDataDeleted)
{
	// clear all deleted assets' checkbox reference before SelectedAssetsData.RemoveAll
	// because AssetsDataDeleted might be itself (or StoredUnusedAssetsData)
	// if u call CheckBoxesOfAssets.Remove after SelectedAssetsData.RemoveAll
	// u won't be able to find any data to delete cause its already deleted
	for (const TSharedPtr AssetDataDeleted : AssetsDataDeleted)
	{
		// StoredUnusedAssetsData and SelectedAssetsData can't use remove here 
		// because AssetsDataDeleted might be itself (or StoredUnusedAssetsData)
		// if u call remove during iterating one TArray, it might crash
		CheckBoxesOfAssets.Remove(AssetDataDeleted);
	}
	auto IsAssetDataDeleted = [&AssetsDataDeleted](const TSharedPtr<FAssetData>& ArrayElement)
	{
		return AssetsDataDeleted.Contains(ArrayElement);
	};
	StoredUnusedAssetsData.RemoveAll(IsAssetDataDeleted);
	SelectedAssetsData.RemoveAll(IsAssetDataDeleted);
	if (AssetsListView.IsValid())
	{
		AssetsListView->RequestListRefresh();
	}
}

#pragma endregion AssetsList
#pragma region AssetsBatchActions

TSharedRef<SHorizontalBox> SAdvancedDeletionWidget::CreateHorizontalBoxAndBatchAssetActionButtons()
{
	constexpr float BatchAssetActionButtonWidth = 15.0f;
	constexpr float BatchAssetActionButtonPadding = 3.0f;
	// 资产批量操作按钮
	TSharedRef<SHorizontalBox> HorizontalBoxForBatchAssetActions = SNew(SHorizontalBox)
	+ SHorizontalBox::Slot().Padding(BatchAssetActionButtonPadding).FillWidth(BatchAssetActionButtonWidth)
	[
		CreateDeleteAllSelectedAssetButton()
	]
	+ SHorizontalBox::Slot().Padding(BatchAssetActionButtonPadding).FillWidth(BatchAssetActionButtonWidth)
	[
		CreateSelectAllAssetButton()
	]
	+ SHorizontalBox::Slot().Padding(BatchAssetActionButtonPadding).FillWidth(BatchAssetActionButtonWidth)
	[
		CreateDeselectAllAssetButton()
	]
	+ SHorizontalBox::Slot().Padding(BatchAssetActionButtonPadding).FillWidth(BatchAssetActionButtonWidth)
	[
		CreateToggleAllAssetButton()
	];
	return HorizontalBoxForBatchAssetActions;
}
TSharedRef<SButton> SAdvancedDeletionWidget::CreateDeleteAllSelectedAssetButton()
{
	LOG_ENTER_FUNCTION();
	TSharedRef<SButton> DeleteAllAssetButton = SNew(SButton).ContentPadding(FMargin(5.0f))
		.OnClicked(this, &SAdvancedDeletionWidget::OnDeleteAllSelectedAssetsButtonClicked);
	DeleteAllAssetButton->SetContent(CreateAssetsBatchActionButtonsTextBlock(TEXT("Delete Selected Assets")));
	return DeleteAllAssetButton;
}

FReply SAdvancedDeletionWidget::OnDeleteAllSelectedAssetsButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (SelectedAssetsData.IsEmpty())
	{
		ShowMessageDialog(TEXT("No selected assets."), false);
		return FReply::Handled();
	}
	TArray<FAssetData> AssetsDataToDeleteArray;
	for (TSharedPtr AssetData : SelectedAssetsData)
	{
		AssetsDataToDeleteArray.Add(*AssetData);
	}
	if (const int32 DeletedAssetsNum = DeleteAssetsAndLog(AssetsDataToDeleteArray); DeletedAssetsNum == AssetsDataToDeleteArray.Num())
	{
		ShowMessageDialog(TEXT("All selected assets have been successfully deleted."), false);
		UpdateAssetsListView(SelectedAssetsData);
	}
	else
	{
		const TArray<TSharedPtr<FAssetData>>& DeletedAssetsData = FilteredOutDeletedAssetsData(
			SelectedAssetsData, DeletedAssetsNum);
		UpdateAssetsListView(DeletedAssetsData);
	}
	return FReply::Handled();
}

TSharedRef<SButton> SAdvancedDeletionWidget::CreateSelectAllAssetButton()
{
	const TCHAR* ActionName = TEXT("Select All Assets");
	TSharedRef<SButton> SelectAllAssetButton = SNew(SButton).ContentPadding(5.0f)
		.OnClicked(this, &SAdvancedDeletionWidget::OnSelectAllAssetButtonClicked);
	SelectAllAssetButton->SetContent(CreateAssetsBatchActionButtonsTextBlock(ActionName));
	return SelectAllAssetButton;
}

FReply SAdvancedDeletionWidget::OnSelectAllAssetButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (CheckBoxesOfAssets.IsEmpty()) return FReply::Handled();
	for (const TPair CheckBoxOfAsset : CheckBoxesOfAssets)
	{
		// select all unselected
		if (!CheckBoxOfAsset.Value->IsChecked()) CheckBoxOfAsset.Value->ToggleCheckedState();
	}
	return FReply::Handled();
}

TSharedRef<SButton> SAdvancedDeletionWidget::CreateDeselectAllAssetButton()
{
	const TCHAR* ActionName = TEXT("Deselect All Assets");
	TSharedRef<SButton> DeselectAllAssetButton = SNew(SButton).ContentPadding(5.0f)
		.OnClicked(this, &SAdvancedDeletionWidget::OnDeselectAllAssetButtonClicked);
	DeselectAllAssetButton->SetContent(CreateAssetsBatchActionButtonsTextBlock(ActionName));
	return DeselectAllAssetButton;
}

FReply SAdvancedDeletionWidget::OnDeselectAllAssetButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (CheckBoxesOfAssets.IsEmpty()) return FReply::Handled();
	for (const TPair CheckBoxOfAsset : CheckBoxesOfAssets)
	{
		// deselect all selected
		if (CheckBoxOfAsset.Value->IsChecked()) CheckBoxOfAsset.Value->ToggleCheckedState();
	}
	return FReply::Handled();
}

TSharedRef<SButton> SAdvancedDeletionWidget::CreateToggleAllAssetButton()
{
	const TCHAR* ActionName = TEXT("Toggle All Assets");
	TSharedRef<SButton> ToggleAllAssetButton = SNew(SButton).ContentPadding(5.0f)
		.OnClicked(this, &SAdvancedDeletionWidget::OnToggleAllAssetButtonClicked);
	ToggleAllAssetButton->SetContent(CreateAssetsBatchActionButtonsTextBlock(ActionName));
	return ToggleAllAssetButton;
}

FReply SAdvancedDeletionWidget::OnToggleAllAssetButtonClicked()
{
	LOG_ENTER_FUNCTION();
	if (CheckBoxesOfAssets.IsEmpty()) return FReply::Handled();
	for (const TPair CheckBoxOfAsset : CheckBoxesOfAssets)
	{
		// toggle all selected
		CheckBoxOfAsset.Value->ToggleCheckedState();
	}
	return FReply::Handled();
}

TSharedRef<STextBlock> SAdvancedDeletionWidget::CreateAssetsBatchActionButtonsTextBlock(const FString& TextToDisplay)
{
	FSlateFontInfo AssetsBatchActionButtonsTextFont = GetEmbossedTextFont();
	AssetsBatchActionButtonsTextFont.Size = 15;
	return SNew(STextBlock)
		.Text(FText::FromString(TextToDisplay))
		.Font(AssetsBatchActionButtonsTextFont)
		.Justification(ETextJustify::Center)
		.ColorAndOpacity(FColor::White);
}
#pragma endregion AssetsBatchActions
