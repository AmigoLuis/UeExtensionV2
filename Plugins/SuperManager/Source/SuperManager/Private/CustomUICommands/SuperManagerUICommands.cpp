// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomUICommands/SuperManagerUICommands.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"
void FSuperManagerUICommands::RegisterCommands()
{
	UI_COMMAND(LockActorSelectionCommandInfo, "LockActorSelection",
	           "Lock all selected objects so that you can't select them until you unlock them.",
	           EUserInterfaceActionType::Button, 
	           FInputChord(EKeys::W, EModifierKey::Alt));
	UI_COMMAND(UnlockActorSelectionCommandInfo, "UnlockActorSelection",
			   "Unlock all selected objects so that you can select them again.",
			   EUserInterfaceActionType::Button, 
			   FInputChord(EKeys::W, EModifierKey::Alt | EModifierKey::Shift));
}
#undef LOCTEXT_NAMESPACE
