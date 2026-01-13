// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidget/AdvancedDeletionWidget.h"

#include "CustomUtilities.h"
#include "DebugHeader.h"

#define CONDITION_LIST_ALL_TEXT TEXT("List all assets")
#define CONDITION_LIST_UNUSED_TEXT TEXT("List unused assets")
#define CONDITION_LIST_SAME_NAME_TEXT TEXT("List assets with the same name")

void SAdvancedDeletionWidget::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
	FSlateFontInfo TitleTextFont = GetEmbossedTextFont();
	TitleTextFont.Size = 30;
	FixUpRedirectors();
	StoredAllAssetsData = InArgs._AllAssetsDataToStore;
	// display all assets by default
	DisplayedAssetsData = StoredAllAssetsData;
	if (StoredAllAssetsData.Num() == 0) return;
	DisplayedSelectedAssetsData.Empty();
	UnDisplayedSelectedAssetsData.Empty();
	
	AssetsAndCheckBoxes.Empty();
	
	AssetListConditionStrings.Add(MakeShared<FString>(CONDITION_LIST_ALL_TEXT));
	AssetListConditionStrings.Add(MakeShared<FString>(CONDITION_LIST_UNUSED_TEXT));
	AssetListConditionStrings.Add(MakeShared<FString>(CONDITION_LIST_SAME_NAME_TEXT));
	
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
				CreateListViewForAssets(DisplayedAssetsData)
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
	// SListView->RequestListRefresh方法调用后，
	// 如果原来的row对应的数据不在ListItemsSource中会取消row的引用，
	// 如果重新再把row对应的数据添加到ListItemsSource中，RequestListRefresh后会重新生成row，而不是复用之前的row
	if (*SelectedListCondition == CONDITION_LIST_ALL_TEXT)
	{
		DisplayedAssetsData = StoredAllAssetsData;
	}
	else if (*SelectedListCondition == CONDITION_LIST_UNUSED_TEXT)
	{
		// 过滤出没有包引用关系的资产
		FilteredOutUnusedAssetsData(StoredAllAssetsData, DisplayedAssetsData);
		UpdateAssetsListViewWhenConditionChanged();
	}
	else if (*SelectedListCondition == CONDITION_LIST_SAME_NAME_TEXT)
	{
		FilteredOutAssetsDataWithSameName(StoredAllAssetsData, DisplayedAssetsData);
		UpdateAssetsListViewWhenConditionChanged();
	}
	
	if (AssetsListView.IsValid())
	{
		AssetsListView->RequestListRefresh();
	}
}

TSharedRef<SWidget> SAdvancedDeletionWidget::OnGenerateAssetListCondition(
	TSharedPtr<FString> ListConditionString)
{
	return SNew(STextBlock).Text(FText::FromString(*ListConditionString));
}

void SAdvancedDeletionWidget::UpdateAssetsListViewWhenConditionChanged()
{
	// 先过滤出没有展示的AssetData
	TArray<TSharedPtr<FAssetData>> UnDisplayedAssetsData;
	for (auto AssetData:StoredAllAssetsData)
	{
		if (!DisplayedAssetsData.Contains(AssetData))
		{
			UnDisplayedAssetsData.Add(AssetData);
		}
	}
	// 更新checkbox集合，删除没有展示的checkbox
	for (auto AssetAndCheckBox : UnDisplayedAssetsData)
	{
		AssetsAndCheckBoxes.Remove(AssetAndCheckBox);
	}
		
	auto RemoveAllSelectedButNotDisplayed = [this](const TSharedPtr<FAssetData>& DisplayedSelectedAssetData)
	{
		const bool bNotDisplayed = !DisplayedAssetsData.Contains(DisplayedSelectedAssetData); 
		PrintInLog(TEXT("RemoveAllSelectedButNotDisplayed"));
		if (bNotDisplayed)
		{
			// 保存已选中但是当前页面不展示的数据
			UnDisplayedSelectedAssetsData.AddUnique(DisplayedSelectedAssetData);
		}
		return bNotDisplayed;
	};
	DisplayedSelectedAssetsData.RemoveAll(RemoveAllSelectedButNotDisplayed);
}

#pragma endregion AssetListConditionComboBoxes

#pragma region AssetsList
TSharedRef<ITableRow> SAdvancedDeletionWidget::OnGenerateListViewRow(TSharedPtr<FAssetData> AssetData,
                                                                     const TSharedRef<STableViewBase>& OwnerTable)
{
	LOG_ENTER_FUNCTION();
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
	
	if (DisplayedSelectedAssetsData.Contains(AssetDataToDisplay))
	{
		// 要生成的CheckBox对应的AssetData是目前选中的并且展示中的（尚不清楚为啥会生成一个展示中的row，可能是ue特殊逻辑）
		// 现在重新生成后恢复为之前的选中，
		// ToggleCheckedState方法触发OnCheckBoxStateChanged事件，自动将调用DisplayedSelectedAssetsData->AddUnique(AssetData)
		// 但是由于已经添加过，AddUnique函数不会重复添加
		// 必须在检查UnDisplayedSelectedAssetsData之前检查，
		// 因为会检查逻辑中会触发OnCheckBoxStateChanged事件，导致添加到DisplayedSelectedAssetsData中，重复ToggleCheckedState
		CreatedCheckBox->ToggleCheckedState();
	}
	if (UnDisplayedSelectedAssetsData.Contains(AssetDataToDisplay))
	{
		// 要生成的CheckBox对应的AssetData是之前选中的但是由于过滤条件导致未展示的，
		// 现在重新生成后恢复为之前的选中，
		// ToggleCheckedState方法触发OnCheckBoxStateChanged事件，自动将AssetData添加回DisplayedSelectedAssetsData
		CreatedCheckBox->ToggleCheckedState();
		UnDisplayedSelectedAssetsData.Remove(AssetDataToDisplay);
	}
	AssetsAndCheckBoxes.Add(AssetDataToDisplay, CreatedCheckBox.ToSharedPtr());
	return CreatedCheckBox;
}

TSharedRef<SButton> SAdvancedDeletionWidget::CreateDeleteOneAssetButton(const TSharedPtr<FAssetData>& AssetDataToDelete)
{
	TSharedRef<SButton> DeletionButton = SNew(SButton).Text(FText::FromString(TEXT("DeleteAsset"))).OnClicked_Lambda(
			[AssetDataToDelete, this]
			{
				if (DeleteAssetsAndLog(*AssetDataToDelete) > 0)
				{
					UpdateAssetsListViewWhenDeletedAssets(AssetDataToDelete);
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
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is checked."), SuperManager::Verbose);
		DisplayedSelectedAssetsData.AddUnique(AssetData);
		break;
	case ECheckBoxState::Unchecked:
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is unchecked."), SuperManager::Verbose);
		DisplayedSelectedAssetsData.Remove(AssetData);
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

void SAdvancedDeletionWidget::UpdateAssetsListViewWhenDeletedAssets(const TSharedPtr<FAssetData>& AssetDataDeleted)
{
	// clear deleted asset's checkbox reference before SelectedAssetsData.Remove
	// because AssetsDataDeleted might be in itself (or StoredUnusedAssetsData)
	// if u call CheckBoxesOfAssets.Remove after SelectedAssetsData.RemoveAll
	// u won't be able to find any data to delete cause its already deleted
	AssetsAndCheckBoxes.Remove(AssetDataDeleted);
	
	DisplayedAssetsData.Remove(AssetDataDeleted);
	StoredAllAssetsData.Remove(AssetDataDeleted);
	DisplayedSelectedAssetsData.Remove(AssetDataDeleted);
	if (AssetsListView.IsValid())
	{
		AssetsListView->RequestListRefresh();
	}
}

void SAdvancedDeletionWidget::UpdateAssetsListViewWhenDeletedAssets(const TArray<TSharedPtr<FAssetData>>& AssetsDataDeleted)
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
		AssetsAndCheckBoxes.Remove(AssetDataDeleted);
	}
	auto IsAssetDataDeleted = [&AssetsDataDeleted](const TSharedPtr<FAssetData>& ArrayElement)
	{
		return AssetsDataDeleted.Contains(ArrayElement);
	};
	DisplayedAssetsData.RemoveAll(IsAssetDataDeleted);
	StoredAllAssetsData.RemoveAll(IsAssetDataDeleted);
	DisplayedSelectedAssetsData.RemoveAll(IsAssetDataDeleted);
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
	if (DisplayedSelectedAssetsData.IsEmpty())
	{
		ShowMessageDialog(TEXT("No selected assets."), false);
		return FReply::Handled();
	}
	TArray<FAssetData> AssetsDataToDeleteArray;
	for (TSharedPtr AssetData : DisplayedSelectedAssetsData)
	{
		AssetsDataToDeleteArray.Add(*AssetData);
	}
	if (const int32 DeletedAssetsNum = DeleteAssetsAndLog(AssetsDataToDeleteArray); DeletedAssetsNum == AssetsDataToDeleteArray.Num())
	{
		ShowMessageDialog(TEXT("All selected assets have been successfully deleted."), false);
		UpdateAssetsListViewWhenDeletedAssets(DisplayedSelectedAssetsData);
	}
	else
	{
		const TArray<TSharedPtr<FAssetData>>& DeletedAssetsData = FilteredOutDeletedAssetsData(
			DisplayedSelectedAssetsData, DeletedAssetsNum);
		UpdateAssetsListViewWhenDeletedAssets(DeletedAssetsData);
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
	if (AssetsAndCheckBoxes.IsEmpty()) return FReply::Handled();
	for (const TPair CheckBoxOfAsset : AssetsAndCheckBoxes)
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
	if (AssetsAndCheckBoxes.IsEmpty()) return FReply::Handled();
	for (const TPair CheckBoxOfAsset : AssetsAndCheckBoxes)
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
	if (AssetsAndCheckBoxes.IsEmpty()) return FReply::Handled();
	for (const TPair CheckBoxOfAsset : AssetsAndCheckBoxes)
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
