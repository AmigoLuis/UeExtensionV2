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
	static const FString DefaultMaterialName;
	static const FString Texture2DPrefix;
public:
	UFUNCTION(BlueprintCallable)
	void CreateMaterialFromSelectedTextures();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CreateMaterialFromSelectedTextures", meta=(EditCondition="bUseCustomMaterialName"))
	FString MaterialNam = DefaultMaterialName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CreateMaterialFromSelectedTextures")
	bool bUseCustomMaterialName = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CreateMaterialFromSelectedTextures", meta=(EditCondition="false"))
	FString SelectedTexture2DPackagePath;
#pragma endregion Quick Material Creation Core
	
#pragma region Quick Material Creation
	bool GetSelectedTexture2DAndSetMaterialNameAndPackagePath(
		TArray<UTexture2D*>& SelectedTexture2Ds);
#pragma endregion Quick Material Creation
};
