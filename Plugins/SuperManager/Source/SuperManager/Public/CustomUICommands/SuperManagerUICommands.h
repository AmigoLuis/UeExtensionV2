// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/**
 * 
 */
class FSuperManagerUICommands : public TCommands<FSuperManagerUICommands>
{
public:
	FSuperManagerUICommands() : TCommands(
		TEXT("SuperManager"), 
		FText::FromString(TEXT("SuperManagerUICommands")), 
		NAME_None,
		TEXT("SuperManager")) {}

	virtual void RegisterCommands() override;
	TSharedPtr<FUICommandInfo> LockActorSelectionCommandInfo;
	TSharedPtr<FUICommandInfo> UnlockActorSelectionCommandInfo;
protected:
};
