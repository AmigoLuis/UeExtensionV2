// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomOutliner/FOutlinerSelectionLock.h"

#include "CustomStyles/FSuperManagerStyle.h"

SHeaderRow::FColumn::FArguments FOutlinerSelectionLock::ConstructHeaderRowColumn()
{
	SHeaderRow::FColumn::FArguments CreatedOutlinerHeaderColumn =
		SHeaderRow::Column(GetColumnID()).FixedWidth(24).HAlignHeader(HAlign_Center).VAlignHeader(VAlign_Center).
		                                  HAlignCell(HAlign_Center).VAlignCell(VAlign_Center).DefaultTooltip(
			                                  FText::FromString(TEXT("Press the icon to lock actors' selection.")))
		[
			SNew(SImage).ColorAndOpacity(FSlateColor::UseForeground()).Image(
				FSuperManagerStyle::GetCreatedSlateStyleSet()->GetBrush(
					FSuperManagerStyle::GetSelectionLockIconName()))
		];
	return CreatedOutlinerHeaderColumn;
}

const TSharedRef<SWidget> FOutlinerSelectionLock::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem,
                                                                      const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	// TSharedRef<SWidget> CreatedWidget;
	return SNullWidget::NullWidget;
}
