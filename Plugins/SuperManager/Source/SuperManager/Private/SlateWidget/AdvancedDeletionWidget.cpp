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

TSharedRef<ITableRow> SAdvancedDeletionWidget::OnGenerateListViewRow(TSharedPtr<FAssetData> AssetDataToDisplay,
                                                                     const TSharedRef<STableViewBase>& OwnerTable)
{
	
	FSlateFontInfo AssetClassTextFont = GetEmbossedTextFont();
	AssetClassTextFont.Size = 10;
	return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		[
			SNew(SHorizontalBox)
			// 1 checkbox
			+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Center).FillWidth(0.05f)
			[
				CreateCheckBox(AssetDataToDisplay)
			]
			// 2 asset class name
			+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Fill).FillWidth(0.2f)
			[
				CreateTextBlock(AssetDataToDisplay->AssetClassPath.GetAssetName().ToString(), AssetClassTextFont)
			]
			// 3 asset name
			+ SHorizontalBox::Slot()
			[
				CreateTextBlock(AssetDataToDisplay->AssetName.ToString(), AssetClassTextFont)
			]
			// 4 delete button
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
