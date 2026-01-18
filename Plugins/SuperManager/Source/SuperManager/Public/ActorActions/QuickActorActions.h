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
	
#pragma region ActorsBatchRandomizeTransform
	UFUNCTION(BlueprintCallable, Category="ActorsBatchRandomizeTransform")
	void ActorsBatchRandomizeTransform();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchRandomizeTransform")
	TMap<EBatchActorActionAxis, FVector2D> AxisAndAngleRangeOfBatchRandomizeTransform =
	{
		{EBatchActorActionAxis::EBatchActorActionXAxis, {-360, 360}},
		{EBatchActorActionAxis::EBatchActorActionYAxis, {-360, 360}},
		{EBatchActorActionAxis::EBatchActorActionZAxis, {-360, 360}}
	};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchRandomizeTransform")
	TMap<EBatchActorActionAxis, FVector2D> AxisAndScaleRangeOfBatchRandomizeTransform =
	{
		{EBatchActorActionAxis::EBatchActorActionXAxis, {0.5, 1.5}},
		{EBatchActorActionAxis::EBatchActorActionYAxis, {0.5, 1.5}},
		{EBatchActorActionAxis::EBatchActorActionZAxis, {0.5, 1.5}}
	};
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchRandomizeTransform")
	TMap<EBatchActorActionAxis, FVector2D> AxisAndOffsetRangeOfBatchRandomizeTransform =
	{
		{EBatchActorActionAxis::EBatchActorActionXAxis, {-10, 10}},
		{EBatchActorActionAxis::EBatchActorActionYAxis, {-10, 10}},
		{EBatchActorActionAxis::EBatchActorActionZAxis, {-10, 10}}
	};
	bool IsRandomRangeParamsValid();
	bool RandomizeActorRotation(AActor* Actor);
	bool RandomizeActorOffset(AActor* Actor);
	bool RandomizeActorScale(AActor* Actor);
#pragma endregion ActorsBatchRandomizeTransform
#pragma region ActorsBatchDuplication
	UFUNCTION(BlueprintCallable, Category="ActorsBatchDuplication")
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
	UFUNCTION(BlueprintCallable, Category="ActorBatchSelection")
	void ActorsBatchSelection();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;
#pragma endregion ActorsBatchSelection
private:
	UPROPERTY()
	UEditorActorSubsystem* EditorActorSubsystem;
	bool GetEditorActorSubsystem();
};
