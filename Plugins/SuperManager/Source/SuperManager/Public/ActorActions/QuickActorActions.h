// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActions.generated.h"

UENUM(BlueprintType)
enum class EBatchActorActionAxis : uint8
{
	EBatchActorActionXAxis UMETA(DisplayName="XAxis"),
	EBatchActorActionYAxis UMETA(DisplayName="YAxis"),
	EBatchActorActionZAxis UMETA(DisplayName="ZAxis"),
	EDefaultMax UMETA(DisplayName="Default Max"),
};

class UEditorActorSubsystem;
/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickActorActions : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:	
	
#pragma region ActorsBatchRandomizeRotation
	UFUNCTION(BlueprintCallable)
	void ActorsBatchRandomizeRotation();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchRandomizeRotation")
	TMap<EBatchActorActionAxis, FVector2D> AxisOfBatchRandomizeRotationAndRangeOfAngle =
	{
		{EBatchActorActionAxis::EBatchActorActionXAxis, {-360, 360}},
		{EBatchActorActionAxis::EBatchActorActionYAxis, {-360, 360}},
		{EBatchActorActionAxis::EBatchActorActionZAxis, {-360, 360}}
	};
	bool IsRandomizeRotationParamsValid();
#pragma endregion ActorsBatchRandomizeRotarion
#pragma region ActorsBatchDuplication
	UFUNCTION(BlueprintCallable)
	void ActorsBatchDuplication();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchDuplication")
	uint8 TimeOfDuplication;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchDuplication")
	float OffsetOfDuplication;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchDuplication")
	EBatchActorActionAxis AxisOfDuplication = EBatchActorActionAxis::EBatchActorActionXAxis;
	bool IsDuplicationParamsValid();
#pragma endregion ActorsBatchDuplication
#pragma region ActorsBatchSelection
	UFUNCTION(BlueprintCallable)
	void ActorsBatchSelection();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;
#pragma endregion ActorsBatchSelection
private:
	UPROPERTY()
	UEditorActorSubsystem* EditorActorSubsystem;
	bool GetEditorActorSubsystem();
};
