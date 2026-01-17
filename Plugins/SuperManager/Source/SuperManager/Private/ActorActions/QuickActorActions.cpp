// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorActions/QuickActorActions.h"

#include "DebugHeader.h"
#include "Subsystems/EditorActorSubsystem.h"

#pragma region ActorsBatchRandomizeRotation
void UQuickActorActions::ActorsBatchRandomizeTransform()
{
	if (!GetEditorActorSubsystem()) return;
	TArray<AActor*> SelectedLevelActors = EditorActorSubsystem->GetSelectedLevelActors();
	if (SelectedLevelActors.Num() < 1)
	{
		ShowMessageDialog(TEXT("Please select at least 1 actor."));
		return;
	}
	if (!IsRandomRangeParamsValid()) return;
	uint8 RotationRandomizedActorNum = 0;
	for (AActor* Actor : SelectedLevelActors)
	{
		if (!Actor) continue;
		const bool RandomizedRotation = RandomizeActorRotation(Actor);
		const bool RandomizedOffset = RandomizeActorOffset(Actor);
		const bool RandomizedScale = RandomizeActorScale(Actor);
		if (RandomizedRotation|| RandomizedOffset || RandomizedScale) ++RotationRandomizedActorNum;
	}
	if (RotationRandomizedActorNum == 0)
	{
		ShowMessageDialog(TEXT("Sorry, no succeeded TransformRandomization."));
	}
	else
	{
		ShowNotifyInfo(FString::Format(TEXT("Successfully randomized {0} actors' transform."), {RotationRandomizedActorNum}) );
	}
}
bool UQuickActorActions::IsRandomRangeParamsValid()
{
	auto IsRangeValid = [](const UE::Math::TVector2<double>& Range)
	{
		if (Range.X > Range.Y)
		{
			ShowMessageDialog(TEXT("left part of Range Of Randomize Range should be smaller"));
			return false;
		}
		return true;
	};
	for (TPair P :AxisAndAngleRangeOfBatchRandomizeTransform)
	{
		if (!IsRangeValid(P.Value)) return false;
	}
	for (TPair P :AxisAndOffsetRangeOfBatchRandomizeTransform)
	{
		if (!IsRangeValid(P.Value)) return false;
	}
	for (TPair P :AxisAndScaleRangeOfBatchRandomizeTransform)
	{
		if (!IsRangeValid(P.Value)) return false;
	}
	return true;
}

bool UQuickActorActions::RandomizeActorRotation(AActor* Actor)
{
	bool ActorRotated = false;
	FRotator RandomRotation = FRotator(0,0,0);
	for (const TPair P :AxisAndAngleRangeOfBatchRandomizeTransform)
	{
		if (P.Key == EBatchActorActionAxis::EBatchActorActionXAxis)
		{
			const double RandomAngle = FMath::RandRange(P.Value.X, P.Value.Y);
			RandomRotation.SetComponentForAxis(EAxis::Type::X, RandomAngle);
			ActorRotated = true;
		}
		if (P.Key == EBatchActorActionAxis::EBatchActorActionYAxis)
		{
			const double RandomAngle = FMath::RandRange(P.Value.X, P.Value.Y);
			RandomRotation.SetComponentForAxis(EAxis::Type::Y, RandomAngle);
			ActorRotated = true;
		}
		if (P.Key == EBatchActorActionAxis::EBatchActorActionZAxis)
		{
			const double RandomAngle = FMath::RandRange(P.Value.X, P.Value.Y);
			RandomRotation.SetComponentForAxis(EAxis::Type::Z, RandomAngle);
			ActorRotated = true;
		}
	}
	if (ActorRotated)
	{
		Actor->AddActorWorldRotation(RandomRotation);
		EditorActorSubsystem->SetActorSelectionState(Actor, true);	
	}
	return ActorRotated;
}

bool UQuickActorActions::RandomizeActorOffset(AActor* Actor)
{
	bool ActorOffset = false;
	FVector RandomOffset = FVector(0,0,0);
	for (const TPair P :AxisAndOffsetRangeOfBatchRandomizeTransform)
	{
		if (P.Key == EBatchActorActionAxis::EBatchActorActionXAxis)
		{
			const double RandomizedOffset = FMath::RandRange(P.Value.X, P.Value.Y);
			RandomOffset.SetComponentForAxis(EAxis::Type::X, RandomizedOffset);
			ActorOffset = true;
		}
		if (P.Key == EBatchActorActionAxis::EBatchActorActionYAxis)
		{
			const double RandomizedOffset = FMath::RandRange(P.Value.X, P.Value.Y);
			RandomOffset.SetComponentForAxis(EAxis::Type::Y, RandomizedOffset);
			ActorOffset = true;
		}
		if (P.Key == EBatchActorActionAxis::EBatchActorActionZAxis)
		{
			const double RandomizedOffset = FMath::RandRange(P.Value.X, P.Value.Y);
			RandomOffset.SetComponentForAxis(EAxis::Type::Z, RandomizedOffset);
			ActorOffset = true;
		}
	}
	if (ActorOffset)
	{
		Actor->AddActorWorldOffset(RandomOffset);
		EditorActorSubsystem->SetActorSelectionState(Actor, true);	
	}
	return ActorOffset;
}

bool UQuickActorActions::RandomizeActorScale(AActor* Actor)
{
	bool ActorScaled = false;
	FVector RandomScale = FVector(0,0,0);
	for (const TPair P :AxisAndScaleRangeOfBatchRandomizeTransform)
	{
		if (P.Key == EBatchActorActionAxis::EBatchActorActionXAxis)
		{
			const double RandomizedScale = FMath::RandRange(P.Value.X, P.Value.Y);
			RandomScale.SetComponentForAxis(EAxis::Type::X, RandomizedScale);
			ActorScaled = true;
		}
		if (P.Key == EBatchActorActionAxis::EBatchActorActionYAxis)
		{
			const double RandomizedScale = FMath::RandRange(P.Value.X, P.Value.Y);
			RandomScale.SetComponentForAxis(EAxis::Type::Y, RandomizedScale);
			ActorScaled = true;
		}
		if (P.Key == EBatchActorActionAxis::EBatchActorActionZAxis)
		{
			const double RandomizedScale = FMath::RandRange(P.Value.X, P.Value.Y);
			RandomScale.SetComponentForAxis(EAxis::Type::Z, RandomizedScale);
			ActorScaled = true;
		}
	}
	if (ActorScaled)
	{
		Actor->SetActorScale3D(RandomScale);
		EditorActorSubsystem->SetActorSelectionState(Actor, true);	
	}
	return ActorScaled;
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
