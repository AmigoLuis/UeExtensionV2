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
	FString MaterialName = DefaultMaterialName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CreateMaterialFromSelectedTextures")
	bool bUseCustomMaterialName = true;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="CreateMaterialFromSelectedTextures", meta=(EditCondition="false"))
	FString SelectedTexture2DPackagePath;
#pragma endregion Quick Material Creation Core
	
#pragma region SupportedTextureNames

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> BaseColorArray = {
		TEXT("_BaseColor"),
		TEXT("_Albedo"),
		TEXT("_Diffuse"),
		TEXT("_diff")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> MetallicArray = {
		TEXT("_Metallic"),
		TEXT("_metal")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> RoughnessArray = {
		TEXT("_Roughness"),
		TEXT("_RoughnessMap"),
		TEXT("_rough")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> NormalArray = {
		TEXT("_Normal"),
		TEXT("_NormalMap"),
		TEXT("_nor")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> AmbientOcclusionArray = {
		TEXT("_AmbientOcclusion"),
		TEXT("_AmbientOcclusionMap"),
		TEXT("_AO")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> ORMArray = {
		TEXT("_arm"),
		TEXT("_OcclusionRoughnessMetallic"),
		TEXT("_ORM")
	};

#pragma endregion SupportedTextureNames

#pragma region Quick Material Creation
	bool GetSelectedTexture2DAndSetMaterialNameAndPackagePath(
		TArray<UTexture2D*>& SelectedTexture2Ds);
	UMaterial* CreateMaterialInSelectedPath(const FString& SelectedPath, const FString& InMaterialName);
#pragma endregion Quick Material Creation
#pragma region Create And Connect Material Nodes
	bool ConnectMaterialNodes(UMaterial* CreatedMaterial, TArray<UTexture2D*>& Textures);
	bool TryConnectToBaseColor(UMaterial* CreatedMaterial, UTexture2D* Texture);
	bool TryConnectToMetallic(UMaterial* CreatedMaterial, UTexture2D* Texture);
	bool TryConnectToNormal(UMaterial* CreatedMaterial, UTexture2D* Texture);
	bool TryConnectToRoughness(UMaterial* CreatedMaterial, UTexture2D* Texture);
	bool TryConnectToAmbientOcclusion(UMaterial* CreatedMaterial, UTexture2D* Texture);
	void AdjustSettingsForMetallicTexture(UTexture2D* Texture);
	void AdjustSettingsForNormalTexture(UTexture2D* Texture);
	void AdjustSettingsForRoughnessTexture(UTexture2D* Texture);
	void AdjustSettingsForAmbientOcclusionTexture(UTexture2D* Texture);
#pragma endregion Create And Connect Material Nodes
};
