// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickMaterialCreation.h"

#include "AssetToolsModule.h"
#include "CustomUtilities.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "MaterialEditingLibrary.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialExpressionTextureSample.h"

#pragma region Quick Material Creation Core
const FString UQuickMaterialCreation::DefaultMaterialName = GetAssetPrefixByAssetClass(UMaterial::StaticClass());
const FString UQuickMaterialCreation::Texture2DPrefix = GetAssetPrefixByAssetClass(UTexture::StaticClass());

void UQuickMaterialCreation::CreateMaterialFromSelectedTextures()
{
	LOG_ENTER_FUNCTION();
	if (bUseCustomMaterialName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(DefaultMaterialName))
		{
			ShowMessageDialog("Please input a valid material name");
			return;
		}
	}
	TArray<FString> SelectedAssetsPathExceptTexture2D;
	TArray<UTexture2D*> SelectedTexture2Ds;
	if (const bool GotTextures = GetSelectedTexture2DAndSetMaterialNameAndPackagePath(SelectedTexture2Ds); 
		!GotTextures) return;
	if (CheckIfAssetNameExistsInFolder(SelectedTexture2DPackagePath, MaterialName))
	{
		ShowMessageDialog(
			FString::Format(TEXT("The {0}:{1} is already existed."), 
				{SYMBOL_NAME_TEXT(MaterialName), MaterialName}));
		return;
	}
	UMaterial* CreatedMaterial = CreateMaterialInSelectedPath(SelectedTexture2DPackagePath, MaterialName);
	if (CreatedMaterial == nullptr)
	{
		ShowMessageDialog(
			FString::Format(TEXT("Failed to create material:{0} at path: {1}"), 
				{MaterialName, SelectedTexture2DPackagePath}));
	}
	if (!ConnectMaterialNodes(CreatedMaterial, SelectedTexture2Ds))
	{
		ShowMessageDialog(TEXT("Failed to ") SYMBOL_NAME_TEXT(ConnectMaterialNodes));
	}
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
			MaterialName = SelectedAssetData.AssetName.ToString();
			MaterialName.RemoveFromStart(Texture2DPrefix);
			MaterialName.InsertAt(0, DefaultMaterialName);
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

UMaterial* UQuickMaterialCreation::CreateMaterialInSelectedPath(const FString& SelectedPath,
	const FString& InMaterialName)
{
	
	const FAssetToolsModule* AssetToolsModule = LoadModulePtrWithLog<FAssetToolsModule>(TEXT("AssetTools"));
	if (!AssetToolsModule) return nullptr;
	UMaterialFactoryNew* MaterialFactoryNew = NewObject<UMaterialFactoryNew>();
	UObject* CreatedAsset = 
		AssetToolsModule->Get().CreateAsset(InMaterialName, SelectedPath, UMaterial::StaticClass(), MaterialFactoryNew);
	UMaterial* CreatedMaterial = Cast<UMaterial>(CreatedAsset);
	return CreatedMaterial;
}
#pragma endregion Quick Material Creation


#pragma region Connect Material Nodes
bool UQuickMaterialCreation::ConnectMaterialNodes(UMaterial* CreatedMaterial, TArray<UTexture2D*>& Textures)
{
	if (!CreatedMaterial) return false;
	uint8 ConnectedNodesNum = 0;
	for (UTexture2D* Texture : Textures)
	{
		if (!Texture) return false;
		if (TryConnectToBaseColor(CreatedMaterial, Texture)) ++ConnectedNodesNum;
		else if (TryConnectToMetallic(CreatedMaterial, Texture)) ++ConnectedNodesNum;
		else if (TryConnectToRoughness(CreatedMaterial, Texture)) ++ConnectedNodesNum;
		else if (TryConnectToNormal(CreatedMaterial, Texture)) ++ConnectedNodesNum;
		else if (TryConnectToAmbientOcclusion(CreatedMaterial, Texture)) ++ConnectedNodesNum;
		else
		{
			ShowNotifyInfo(
			FString::Format(TEXT("Failed to connect texture: {0}."
						"\nPlease check if name contains any of \"Supported Texture Names\""
						" or if too many textures are selected."), 
				{Texture->GetName()}));
		}
	}
	if (ConnectedNodesNum > 0)
	{
		ShowMessageDialog(
			FString::Format(TEXT("Successfully connected {0} nodes."), 
				{ConnectedNodesNum}));
		return true;
	}
	return false;
}

using UTextureSampleNode = UMaterialExpressionTextureSample;
bool UQuickMaterialCreation::TryConnectToBaseColor(UMaterial* CreatedMaterial, UTexture2D* Texture)
{
	if (CreatedMaterial->HasBaseColorConnected()) return false;
	for (const FString& BaseColorName : BaseColorArray)
	{
		if (Texture->GetName().Contains(BaseColorName))
		{
			UTextureSampleNode* TextureSample = Cast<UTextureSampleNode>(
				UMaterialEditingLibrary::CreateMaterialExpression(
					CreatedMaterial,
					UMaterialExpressionTextureSample::StaticClass(),
					-600, -300 // 节点位置X、Y
				));
			TextureSample->Texture = Texture;
			
			// 连接到 BaseColor 输入
			UMaterialEditingLibrary::ConnectMaterialProperty(
				TextureSample,
				TEXT(""),
				MP_BaseColor
			);
			CreatedMaterial->PostEditChange();
			CreatedMaterial->MarkPackageDirty();
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreation::TryConnectToMetallic(UMaterial* CreatedMaterial, UTexture2D* Texture)
{
	if (CreatedMaterial->HasMetallicConnected()) return false;
	for (const FString& MetallicName : MetallicArray)
	{
		if (Texture->GetName().Contains(MetallicName))
		{
			AdjustSettingsForMetallicTexture(Texture);
			UTextureSampleNode* TextureSample = Cast<UTextureSampleNode>(
				UMaterialEditingLibrary::CreateMaterialExpression(
					CreatedMaterial,
					UMaterialExpressionTextureSample::StaticClass(),
					-600, 0 // 节点位置X、Y
				));
			TextureSample->Texture = Texture;
			// 金属色的特殊设置
			TextureSample->SamplerType = SAMPLERTYPE_LinearColor;
			
			// 连接到 Metallic 输入
			UMaterialEditingLibrary::ConnectMaterialProperty(
				TextureSample,
				TEXT(""),
				MP_Metallic
			);
			CreatedMaterial->PostEditChange();
			CreatedMaterial->MarkPackageDirty();
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreation::TryConnectToNormal(UMaterial* CreatedMaterial, UTexture2D* Texture)
{
	if (CreatedMaterial->HasNormalConnected()) return false;
	for (const FString& NormalName : NormalArray)
	{
		if (Texture->GetName().Contains(NormalName))
		{
			AdjustSettingsForNormalTexture(Texture);
			UTextureSampleNode* TextureSample = Cast<UTextureSampleNode>(
				UMaterialEditingLibrary::CreateMaterialExpression(
					CreatedMaterial,
					UMaterialExpressionTextureSample::StaticClass(),
					-600, 600 // 节点位置X、Y
				));
			TextureSample->Texture = Texture;
			// 金属色的特殊设置
			TextureSample->SamplerType = SAMPLERTYPE_Normal;
			
			// 连接到 Metallic 输入
			UMaterialEditingLibrary::ConnectMaterialProperty(
				TextureSample,
				TEXT(""),
				MP_Normal
			);
			CreatedMaterial->PostEditChange();
			CreatedMaterial->MarkPackageDirty();
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreation::TryConnectToRoughness(UMaterial* CreatedMaterial, UTexture2D* Texture)
{
	if (CreatedMaterial->HasRoughnessConnected()) return false;
	for (const FString& RoughnessName : RoughnessArray)
	{
		if (Texture->GetName().Contains(RoughnessName))
		{
			AdjustSettingsForRoughnessTexture(Texture);
			UTextureSampleNode* TextureSample = Cast<UTextureSampleNode>(
				UMaterialEditingLibrary::CreateMaterialExpression(
					CreatedMaterial,
					UMaterialExpressionTextureSample::StaticClass(),
					-600, 300 // 节点位置X、Y
				));
			TextureSample->Texture = Texture;
			// 金属色的特殊设置
			TextureSample->SamplerType = SAMPLERTYPE_LinearColor;
			
			// 连接到 Metallic 输入
			UMaterialEditingLibrary::ConnectMaterialProperty(
				TextureSample,
				TEXT(""),
				MP_Roughness
			);
			CreatedMaterial->PostEditChange();
			CreatedMaterial->MarkPackageDirty();
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreation::TryConnectToAmbientOcclusion(UMaterial* CreatedMaterial, UTexture2D* Texture)
{
	if (CreatedMaterial->HasAmbientOcclusionConnected()) return false;
	for (const FString& AmbientOcclusionName : AmbientOcclusionArray)
	{
		if (Texture->GetName().Contains(AmbientOcclusionName))
		{
			AdjustSettingsForAmbientOcclusionTexture(Texture);
			UTextureSampleNode* TextureSample = Cast<UTextureSampleNode>(
				UMaterialEditingLibrary::CreateMaterialExpression(
					CreatedMaterial,
					UMaterialExpressionTextureSample::StaticClass(),
					-600, 0 // 节点位置X、Y
				));
			TextureSample->Texture = Texture;
			// 金属色的特殊设置
			TextureSample->SamplerType = SAMPLERTYPE_LinearColor;
			
			// 连接到 Metallic 输入
			UMaterialEditingLibrary::ConnectMaterialProperty(
				TextureSample,
				TEXT(""),
				MP_AmbientOcclusion
			);
			CreatedMaterial->PostEditChange();
			CreatedMaterial->MarkPackageDirty();
			return true;
		}
	}
	return false;
}

void UQuickMaterialCreation::AdjustSettingsForMetallicTexture(UTexture2D* Texture)
{
	Texture->CompressionSettings = TC_Default;
	Texture->SRGB = false;
	Texture->PostEditChange();
	Texture->MarkPackageDirty();
}

void UQuickMaterialCreation::AdjustSettingsForNormalTexture(UTexture2D* Texture)
{
	Texture->CompressionSettings = TC_Normalmap;
	// Texture->SRGB = false;
	Texture->PostEditChange();
	Texture->MarkPackageDirty();
}

void UQuickMaterialCreation::AdjustSettingsForRoughnessTexture(UTexture2D* Texture)
{
	Texture->CompressionSettings = TC_Default;
	Texture->SRGB = false;
	Texture->PostEditChange();
	Texture->MarkPackageDirty();
}

void UQuickMaterialCreation::AdjustSettingsForAmbientOcclusionTexture(UTexture2D* Texture)
{
	Texture->CompressionSettings = TC_Default;
	Texture->SRGB = false;
	Texture->PostEditChange();
	Texture->MarkPackageDirty();
}
#pragma endregion Connect Material Nodes
