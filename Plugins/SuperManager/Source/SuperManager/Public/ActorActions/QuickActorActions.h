// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActions.generated.h"

UENUM(BlueprintType)
enum class EDuplicationAxis : uint8
{
	EDuplicateAlongXAxis UMETA(DisplayName="DuplicateAlongXAxis"),
	EDuplicateAlongYAxis UMETA(DisplayName="DuplicateAlongYAxis"),
	EDuplicateAlongZAxis UMETA(DisplayName="DuplicateAlongZAxis"),
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
	
#pragma region ActorsBatchDuplication
	UFUNCTION(BlueprintCallable)
	void ActorsBatchDuplication();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchDuplication")
	uint8 TimeOfDuplication;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchDuplication")
	float OffsetOfDuplication;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorsBatchDuplication")
	EDuplicationAxis AxisOfDuplication = EDuplicationAxis::EDuplicateAlongXAxis;
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
