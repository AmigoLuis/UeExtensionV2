// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidget/AdvancedDeletionWidget.h"

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
		[
			SNew(SHorizontalBox)
		]
		
		+ SVerticalBox::Slot().VAlign(VAlign_Fill) 
		[ // 资产展示列表
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SNew(SListView<TSharedPtr<FAssetData>>)
				.ListItemsSource(&StoredUnusedAssetsData)
				.OnGenerateRow(this, &SAdvancedDeletionWidget::OnGenerateListViewRow)
			]
		]
		
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SHorizontalBox)
		]
	];
}

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
				CreateCheckBox(AssetData)
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
				CreateDeletionButton(AssetData)
			]
		];
}

TSharedRef<SCheckBox> SAdvancedDeletionWidget::CreateCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> CreatedCheckBox = SNew(SCheckBox)
		.Type(ESlateCheckBoxType::Type::CheckBox)
		.OnCheckStateChanged(this, &SAdvancedDeletionWidget::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);
	return CreatedCheckBox;
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

TSharedRef<SButton> SAdvancedDeletionWidget::CreateDeletionButton(const TSharedPtr<FAssetData>& AssetDataToDelete)
{
	return SNew(SButton).Text(FText::FromString(TEXT("DeleteAsset"))).OnClicked_Lambda([AssetDataToDelete]
		{
			PrintInLog(AssetDataToDelete->AssetName.ToString() + TEXT(" is deleted."),
				SuperManager::Display);
			return FReply::Handled();
		});
}

void SAdvancedDeletionWidget::OnCheckBoxStateChanged(ECheckBoxState CheckBoxState, TSharedPtr<FAssetData> AssetData)
{
	switch (CheckBoxState)
	{
		case ECheckBoxState::Checked:
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is checked."), SuperManager::Display);
		break;
	case ECheckBoxState::Unchecked:
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is unchecked."), SuperManager::Display);
		break;
	case ECheckBoxState::Undetermined:
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is undetermined."), SuperManager::Display);
		break;
		default:
		PrintInLog(AssetData->AssetName.ToString() + TEXT(" is unknown."), SuperManager::Display);
		break;
	}
}
FSlateFontInfo SAdvancedDeletionWidget::GetEmbossedTextFont()
{
	return FCoreStyle::Get().GetFontStyle(TEXT("EmbossedText"));
}
