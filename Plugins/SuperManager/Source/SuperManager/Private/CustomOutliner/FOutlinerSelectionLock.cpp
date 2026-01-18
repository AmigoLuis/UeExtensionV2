// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomOutliner/FOutlinerSelectionLock.h"

#include "ActorTreeItem.h"
#include "CustomUtilities.h"
#include "SuperManager.h"
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
	const FActorTreeItem* ActorTreeItem = TreeItem->CastTo<FActorTreeItem>();
	if (!ActorTreeItem || !ActorTreeItem->IsValid()) return SNullWidget::NullWidget;
	const bool IsActorLocked = ActorTreeItem->Actor->ActorHasTag(FSuperManagerModule::LockedObjectSelectionTag);

	const FCheckBoxStyle& CheckBoxStyle = FSuperManagerStyle::GetCreatedSlateStyleSet()->GetWidgetStyle<FCheckBoxStyle>(
		FSuperManagerStyle::GetSelectionLockToggleButtonIconName());
	TSharedRef<SWidget> CreatedWidget = SNew(SCheckBox)
		.HAlign(HAlign_Center)
		.IsChecked(IsActorLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.Visibility(EVisibility::Visible)
		.Type(ESlateCheckBoxType::Type::ToggleButton)
		.Style(&CheckBoxStyle)
		.OnCheckStateChanged(this, &FOutlinerSelectionLock::OnActorSelectionLockCheckBoxStateChanged,
		                     ActorTreeItem->Actor);
	return CreatedWidget;
}

void FOutlinerSelectionLock::OnActorSelectionLockCheckBoxStateChanged(ECheckBoxState NewCheckBoxState,
	TWeakObjectPtr<AActor> ChangedActor)
{
	FSuperManagerModule* SuperManagerModule = LoadModulePtrWithLog<FSuperManagerModule>(TEXT("SuperManager"));
	if (!SuperManagerModule || !ChangedActor.IsValid()) return;
	switch (NewCheckBoxState) {
	case ECheckBoxState::Unchecked:
		SuperManagerModule->SetObjectSelectionLockState(ChangedActor.Get(), false);
		break;
	case ECheckBoxState::Checked:
		SuperManagerModule->SetObjectSelectionLockState(ChangedActor.Get(), true);
		break;
	case ECheckBoxState::Undetermined:
		break;
	}
}
