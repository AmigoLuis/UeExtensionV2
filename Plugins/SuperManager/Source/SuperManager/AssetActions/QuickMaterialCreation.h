// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreation.generated.h"

/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickMaterialCreation : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
#pragma region Quick Material Creation Core
	UFUNCTION(BlueprintCallable)
	void CreateMaterialFromSelectedTextures();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CreateMaterialFromSelectedTextures")
	FString MaterialNamePrefix = TEXT("M_");
#pragma endregion Quick Material Creation Core
};
