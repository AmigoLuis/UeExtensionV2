// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorActions/QuickActorActions.h"

#include "DebugHeader.h"
#include "Subsystems/EditorActorSubsystem.h"

#pragma region ActorsBatchRandomizeRotation
void UQuickActorActions::ActorsBatchRandomizeRotation()
{
	if (!GetEditorActorSubsystem()) return;
	TArray<AActor*> SelectedLevelActors = EditorActorSubsystem->GetSelectedLevelActors();
	if (SelectedLevelActors.Num() < 1)
	{
		ShowMessageDialog(TEXT("Please select at least 1 actor."));
		return;
	}
	if (!IsRandomizeRotationParamsValid()) return;
	uint8 RotationRandomizedActorNum = 0;
	for (AActor* Actor : SelectedLevelActors)
	{
		if (!Actor) continue;
		FRotator RandomRotation = FRotator(0,0,0);
		for (const TPair P :AxisOfBatchRandomizeRotationAndRangeOfAngle)
		{
			if (P.Key == EBatchActorActionAxis::EBatchActorActionXAxis)
			{
				const double RandomAngle = FMath::RandRange(P.Value.X, P.Value.Y);
				RandomRotation.SetComponentForAxis(EAxis::Type::X, RandomAngle);
			}
			if (P.Key == EBatchActorActionAxis::EBatchActorActionYAxis)
			{
				const double RandomAngle = FMath::RandRange(P.Value.X, P.Value.Y);
				RandomRotation.SetComponentForAxis(EAxis::Type::Y, RandomAngle);
			}
			if (P.Key == EBatchActorActionAxis::EBatchActorActionZAxis)
			{
				const double RandomAngle = FMath::RandRange(P.Value.X, P.Value.Y);
				RandomRotation.SetComponentForAxis(EAxis::Type::Z, RandomAngle);
			}
		}
		Actor->AddActorWorldRotation(RandomRotation);
		EditorActorSubsystem->SetActorSelectionState(Actor, true);
		++RotationRandomizedActorNum;
	}
	if (RotationRandomizedActorNum == 0)
	{
		ShowMessageDialog(TEXT("Sorry, no succeeded RotationRandomization."));
	}
	else
	{
		ShowNotifyInfo(FString::Format(TEXT("Successfully randomized {0} actors' rotation."), {RotationRandomizedActorNum}) );
	}
}
bool UQuickActorActions::IsRandomizeRotationParamsValid()
{
	if (AxisOfBatchRandomizeRotationAndRangeOfAngle.IsEmpty())
	{
		ShowMessageDialog(TEXT("Please add at least 1 axis and its RangeOfAngle to batch randomize rotation."));
		return false;
	}
	for (TPair P :AxisOfBatchRandomizeRotationAndRangeOfAngle)
	{
		const UE::Math::TVector2<double>& RangeOfRandomizeRotationAngle = P.Value;
		if (RangeOfRandomizeRotationAngle.X > RangeOfRandomizeRotationAngle.Y)
		{
			ShowMessageDialog(TEXT("left part of Range Of Randomize RotationAngle should be smaller"));
			return false;
		}
	}
	return true;
}

#pragma endregion ActorsBatchRandomizeRotation

#pragma region ActorsBatchDuplication
void UQuickActorActions::ActorsBatchDuplication()
{
	if (!GetEditorActorSubsystem()) return;
	TArray<AActor*> SelectedLevelActors = EditorActorSubsystem->GetSelectedLevelActors();
	if (SelectedLevelActors.Num() < 1)
	{
		ShowMessageDialog(TEXT("Please select at least 1 actor."));
		return;
	}
	if (!IsDuplicationParamsValid()) return;
	uint8 DuplicatedActorNum = 0;
	for (AActor* Actor : SelectedLevelActors)
	{
		if (!Actor) continue;
		for (uint8 i = 0; i < TimeOfDuplication; ++i)
		{
			AActor* DuplicatedActor = EditorActorSubsystem->DuplicateActor(Actor, Actor->GetWorld());
			if (!DuplicatedActor) continue;
			float CurrentOffset = (i+1)*OffsetOfDuplication;
			switch (AxisOfDuplication)
			{
			case EBatchActorActionAxis::EBatchActorActionXAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(CurrentOffset, 0.0f, 0.0f));
				break;
			case EBatchActorActionAxis::EBatchActorActionYAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(0.0f, CurrentOffset, 0.0f));
				break;
			case EBatchActorActionAxis::EBatchActorActionZAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(0.0f, 0.0f, CurrentOffset));
				break;
			case EBatchActorActionAxis::EDefaultMax:
				break;
			}
			EditorActorSubsystem->SetActorSelectionState(DuplicatedActor, true);
			++DuplicatedActorNum;
		}
	}
	if (DuplicatedActorNum == 0)
	{
		ShowMessageDialog(TEXT("Sorry, no succeeded duplication."));
	}
	else
	{
		ShowNotifyInfo(FString::Format(TEXT("Successfully duplicated {0} actors."), {DuplicatedActorNum}) );
	}
}
bool UQuickActorActions::IsDuplicationParamsValid()
{
	if (TimeOfDuplication < 1)
	{
		ShowMessageDialog(TEXT("Please set at least 1 time to duplicate."));
		return false;
	}
	if (OffsetOfDuplication < 0.001f)
	{
		ShowMessageDialog(TEXT("Please set at least 0.01 offset to duplicate."));
		return false;
	}
	return true;
}
#pragma endregion ActorsBatchDuplication

#pragma region ActorsBatchSelection
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
#pragma endregion ActorsBatchSelection

bool UQuickActorActions::GetEditorActorSubsystem()
{
	if (EditorActorSubsystem == nullptr)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}
	return EditorActorSubsystem != nullptr;
}
