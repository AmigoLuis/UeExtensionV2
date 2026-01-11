// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidget/AdvancedDeletionWidget.h"

#include "CustomUtilities.h"
#include "DebugHeader.h"

void SAdvancedDeletionWidget::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
	FSlateFontInfo TitleTextFont = GetEmbossedTextFont();
	TitleTextFont.Size = 30;

	StoredUnusedAssetsData = InArgs._UnusedAssetsDataToStore;
	if (StoredUnusedAssetsData.Num() == 0) return;
	
	ChildSlot[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight() // 标题slot
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Advance Deletion")))
			.Font(TitleTextFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]
		
		+ SVerticalBox::Slot().AutoHeight()
		[ // 资产展示列表
			SNew(SHorizontalBox)
		]
		
		+ SVerticalBox::Slot().VAlign(VAlign_Fill) 
		[ // 资产展示列表
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				CreateListViewForAssets(StoredUnusedAssetsData)
			]
		]
		
		+ SVerticalBox::Slot().AutoHeight()
		[ // 资产批量操作按钮
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().Padding(3.0f).FillWidth(10.0f)
			[
				CreateDeleteAllSelectedAssetButton(SelectedAssetsData)
			]
			+ SHorizontalBox::Slot().Padding(3.0f).FillWidth(10.0f)
			[
				CreateSelectAllAssetButton(SelectedAssetsData)
			]
			+ SHorizontalBox::Slot().Padding(3.0f).FillWidth(10.0f)
			[
				CreateDeselectAllAssetButton(SelectedAssetsData)
			]
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

#pragma region AssetsList
TSharedRef<ITableRow> SAdvancedDeletionWidget::OnGenerateListViewRow(TSharedPtr<FAssetData> AssetData,
                                                                     const TSharedRef<STableViewBase>& OwnerTable)
{
	FSlateFontInfo AssetClassTextFont = GetEmbossedTextFont();
	AssetClassTextFont.Size = 10;
	return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(3.0f))
		[
			SNew(SHorizontalBox)
			// 1 checkbox
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
	StoredUnusedAssetsData.Remove(AssetDataDeleted);
	SelectedAssetsData.Remove(AssetDataDeleted);
	if (AssetsListView.IsValid())
	{
		AssetsListView->RequestListRefresh();
	}
}

void SAdvancedDeletionWidget::UpdateAssetsListView(const TArray<TSharedPtr<FAssetData>>& AssetsDataDeleted)
{
	auto Contains = [&AssetsDataDeleted](const TSharedPtr<FAssetData>& ArrayElement)
	{
		return AssetsDataDeleted.Contains(ArrayElement);
	};
	StoredUnusedAssetsData.RemoveAll(Contains);
	SelectedAssetsData.RemoveAll(Contains);
	if (AssetsListView.IsValid())
	{
		AssetsListView->RequestListRefresh();
	}
}
#pragma endregion AssetsList
#pragma region AssetsBatchActions
TSharedRef<SButton> SAdvancedDeletionWidget::CreateDeleteAllSelectedAssetButton(
	const TArray<TSharedPtr<FAssetData>>& AssetsDataToDelete)
{
	LOG_ENTER_FUNCTION();
	TSharedRef<SButton> DeleteAllAssetButton = SNew(SButton).ContentPadding(FMargin(5.0f))
		.OnClicked_Lambda(
			[this, &AssetsDataToDelete]
			{
				if (AssetsDataToDelete.IsEmpty())
				{
					ShowMessageDialog(TEXT("No selected assets."), false);
					return FReply::Handled();
				}
				TArray<FAssetData> AssetsDataToDeleteArray;
				for (TSharedPtr AssetData : AssetsDataToDelete)
				{
					AssetsDataToDeleteArray.Add(*AssetData);
				}
				const int32 DeletedAssetsNum = DeleteAssetsAndLog(AssetsDataToDeleteArray); 
				if (DeletedAssetsNum == AssetsDataToDeleteArray.Num())
				{
					ShowMessageDialog(TEXT("All selected assets have been successfully deleted."), false);
					UpdateAssetsListView(AssetsDataToDelete);
				}
				else
				{
					const TArray<TSharedPtr<FAssetData>>& DeletedAssetsData = FilteredOutDeletedAssetsData(
						AssetsDataToDelete, DeletedAssetsNum);
					UpdateAssetsListView(DeletedAssetsData);
				}
				return FReply::Handled();
			});
	DeleteAllAssetButton->SetContent(CreateAssetsBatchActionButtonsTextBlock(TEXT("Delete All Selected Assets")));
	return DeleteAllAssetButton;
}

TSharedRef<SButton> SAdvancedDeletionWidget::CreateSelectAllAssetButton(
	const TArray<TSharedPtr<FAssetData>>& AssetsDataToDelete)
{
	const TCHAR* ActionName = TEXT("Select All Assets");
	TSharedRef<SButton> DeleteAllAssetButton = SNew(SButton).ContentPadding(5.0f)
		.OnClicked_Lambda(
			[ActionName, this]
			{
				// if (DeleteAssetsAndLog(*AssetDataToDelete) > 0)
				// {
				// 	UpdateAssetsListView(AssetDataToDelete);
				// }
				PrintInLog(ActionName, SuperManager::Display);
				return FReply::Handled();
			});
	DeleteAllAssetButton->SetContent(CreateAssetsBatchActionButtonsTextBlock(ActionName));
	return DeleteAllAssetButton;
}

TSharedRef<SButton> SAdvancedDeletionWidget::CreateDeselectAllAssetButton(
	const TArray<TSharedPtr<FAssetData>>& AssetsDataToDelete)
{
	const TCHAR* ActionName = TEXT("Deselect All Assets");
	TSharedRef<SButton> DeleteAllAssetButton = SNew(SButton).ContentPadding(5.0f)
		.OnClicked_Lambda(
			[ActionName, this]
			{
				// if (DeleteAssetsAndLog(*AssetDataToDelete) > 0)
				// {
				// 	UpdateAssetsListView(AssetDataToDelete);
				// }
				PrintInLog(ActionName, SuperManager::Display);
				return FReply::Handled();
			});
	DeleteAllAssetButton->SetContent(CreateAssetsBatchActionButtonsTextBlock(ActionName));
	return DeleteAllAssetButton;
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
