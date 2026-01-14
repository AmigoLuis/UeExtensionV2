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
#pragma region Quick Material Creation Core
private:
	const TCHAR* DefaultMaterialName = TEXT("M_");
public:
	UFUNCTION(BlueprintCallable)
	void CreateMaterialFromSelectedTextures();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CreateMaterialFromSelectedTextures", meta=(EditCondition="bUseCustomMaterialName"))
	FString MaterialNam = DefaultMaterialName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CreateMaterialFromSelectedTextures")
	bool bUseCustomMaterialName = true;
#pragma endregion Quick Material Creation Core
};
