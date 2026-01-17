// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorActions/QuickActorActions.h"

#include "DebugHeader.h"
#include "Subsystems/EditorActorSubsystem.h"

void UQuickActorActions::ActorsBatchSelection()
{
	if (!GetEditorActorSubsystem()) return;
	TArray<AActor*> SelectedLevelActors = EditorActorSubsystem->GetSelectedLevelActors();
	if (SelectedLevelActors.Num() != 1)
	{
		ShowMessageDialog(TEXT("You can select 1 and only 1 actor at a time."));
		return;
	}
	uint8 SimilarNameActorNum = 0;
	const FString& SimilarName = SelectedLevelActors[0]->GetActorLabel().LeftChop(4);
	for (const TArray<AActor*>& AllLevelActors = EditorActorSubsystem->GetAllLevelActors(); 
		AActor* Actor : AllLevelActors)
	{
		if (!Actor) continue;
		if (Actor->GetActorLabel().Contains(SimilarName, SearchCase))
		{
			EditorActorSubsystem->SetActorSelectionState(Actor, true);
			++SimilarNameActorNum;
		}
	}
	if (SimilarNameActorNum == 0)
	{
		ShowMessageDialog(FString::Format(TEXT("No actor with similar name :{1}  have been selected."),
			{SimilarName}));
	}
	else
	{
		
		ShowNotifyInfo(FString::Format(TEXT("{0} Actors with similar name :{1}  have been selected."),
			{SimilarNameActorNum, SimilarName}));
	}
}

bool UQuickActorActions::GetEditorActorSubsystem()
{
	if (EditorActorSubsystem == nullptr)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}
	return EditorActorSubsystem != nullptr;
}
