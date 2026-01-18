// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ISceneOutlinerColumn.h"
/**
 * 
 */
class FOutlinerSelectionLock : public ISceneOutlinerColumn
{
public:
	FOutlinerSelectionLock(ISceneOutliner& SceneOutliner){}
	virtual FName GetColumnID() override {return GetID();}
	static FName GetID() {return FName("SelectionLocked");}
	virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;
	virtual const TSharedRef<SWidget> ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem,
		const STableRow<FSceneOutlinerTreeItemPtr>& Row) override;
	void OnActorSelectionLockCheckBoxStateChanged(ECheckBoxState NewCheckBoxState, TWeakObjectPtr<AActor> ChangedActor);
};
