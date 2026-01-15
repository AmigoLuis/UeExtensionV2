// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickMaterialCreation.h"

#include "CustomUtilities.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"

#pragma region Quick Material Creation Core
const FString UQuickMaterialCreation::DefaultMaterialName = GetAssetPrefixByAssetClass(UMaterial::StaticClass());
const FString UQuickMaterialCreation::Texture2DPrefix = GetAssetPrefixByAssetClass(UTexture::StaticClass());

void UQuickMaterialCreation::CreateMaterialFromSelectedTextures()
{
	LOG_ENTER_FUNCTION();
	if (bUseCustomMaterialName)
	{
		if (MaterialNam.IsEmpty() || MaterialNam.Equals(DefaultMaterialName))
		{
			ShowMessageDialog("Please input a valid material name");
			return;
		}
	}
	TArray<FString> SelectedAssetsPathExceptTexture2D;
	TArray<UTexture2D*> SelectedTexture2Ds;
	if (const bool GotTextures = GetSelectedTexture2DAndSetMaterialNameAndPackagePath(SelectedTexture2Ds); 
		!GotTextures) return;
	
}
#pragma endregion Quick Material Creation Core

#pragma region Quick Material Creation
bool UQuickMaterialCreation::GetSelectedTexture2DAndSetMaterialNameAndPackagePath(
	TArray<UTexture2D*>& SelectedTexture2Ds)
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FString> SelectedAssetsPathExceptTexture2D;
	bool bSetMaterialName = false;
	bool bSetTexture2DPackagePath = false;
	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		UObject* SelectedAssetObject = SelectedAssetData.GetAsset();
		UTexture2D* SelectedTexture2D = Cast<UTexture2D>(SelectedAssetObject);
		if (SelectedTexture2D == nullptr)
		{
			SelectedAssetsPathExceptTexture2D.Add(SelectedAssetData.AssetName.ToString());
			continue;
		}
		SelectedTexture2Ds.Add(SelectedTexture2D);

		if (!bSetTexture2DPackagePath)
		{
			SelectedTexture2DPackagePath = SelectedAssetData.PackagePath.ToString();
			bSetTexture2DPackagePath = true;
		}
		if (!bUseCustomMaterialName && !bSetMaterialName)
		{
			MaterialNam = SelectedAssetData.AssetName.ToString();
			MaterialNam.RemoveFromStart(Texture2DPrefix);
			MaterialNam.InsertAt(0, DefaultMaterialName);
			bSetMaterialName = true;
		}
	}
	if (SelectedAssetsPathExceptTexture2D.Num() > 0)
	{
		FString NotifyInformation = TEXT("Can't process ") + FString::FromInt(SelectedAssetsPathExceptTexture2D.Num()) +
			TEXT(" non texture2d assets, already ignored them:\n");
		for (const FString& SelectedAssetPath : SelectedAssetsPathExceptTexture2D)
		{
			NotifyInformation += TEXT("\t") + SelectedAssetPath + TEXT("\n");
		}
		ShowNotifyInfo(NotifyInformation);
	}
	if (SelectedTexture2Ds.IsEmpty())
	{
		ShowMessageDialog("Please select at least 1 texture2D", false);
		return false;
	}
	return true;
}
#pragma endregion Quick Material Creation