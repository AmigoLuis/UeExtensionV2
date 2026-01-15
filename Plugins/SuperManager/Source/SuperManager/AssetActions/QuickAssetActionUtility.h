// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "QuickAssetActionUtility.generated.h"

/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickAssetActionUtility : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor, Category = "SuperManager")
	static void DuplicateAssets(int32 NumOfDuplicates);
	UFUNCTION(CallInEditor, Category = "SuperManager")
	void AutoAddPrefixes();
	UFUNCTION(CallInEditor, Category = "SuperManager")
	static void RemoveUnusedAssets();
private:
	static void SpecialProcessAssetName(const UObject* Asset, FString& OldAssetName);
};
