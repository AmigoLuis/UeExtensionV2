// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActions.generated.h"

class UEditorActorSubsystem;
/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickActorActions : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:	
	UFUNCTION(BlueprintCallable)
	void ActorsBatchSelection();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;
private:
	UPROPERTY()
	UEditorActorSubsystem* EditorActorSubsystem;
	bool GetEditorActorSubsystem();
};
