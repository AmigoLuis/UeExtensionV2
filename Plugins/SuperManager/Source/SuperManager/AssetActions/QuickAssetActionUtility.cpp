// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickAssetActionUtility.h"

#include "CustomUtilities.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

void UQuickAssetActionUtility::DuplicateAssets(const int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		const EAppReturnType::Type UserChoice = ShowMessageDialog(
			TEXT("Please provide a valid number of duplicates as parameter."));
		if (UserChoice != EAppReturnType::Yes)
		{
		}
		return;
	}
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 CounterOfSucceededDuplication = 0;
	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int32 i = 0; i < NumOfDuplicates; i++)
		{
			const FString SourceAssetPath = SelectedAssetData.GetObjectPathString();;
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") +
				FString::FromInt(i + 1);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(),
			                                            NewDuplicatedAssetName);
			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewPathName, false);
				++CounterOfSucceededDuplication;
			}
		}
	}
	if (CounterOfSucceededDuplication > 0)
	{
		ShowNotifyInfo(
			TEXT("Successfully duplicated ") + FString::FromInt(CounterOfSucceededDuplication) + TEXT(" assets"));
	}
}

void UQuickAssetActionUtility::AutoAddPrefixes()
{
	auto SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 CounterOfSucceededAdding = 0;
	for (const auto SelectedAsset : SelectedAssets)
	{
		if (!SelectedAsset) continue;
		const auto PrefixFound = AssetTypeToPrefixMapping.Find(SelectedAsset->GetClass());
		if (!PrefixFound || PrefixFound->IsEmpty())
		{
			ShowMessageDialog(TEXT("Failed to find Prefix for class: ") + SelectedAsset->GetClass()->GetName());
			continue;
		}
		FString OldName = SelectedAsset->GetName();
		if (OldName.StartsWith(*PrefixFound))
		{
			ShowMessageDialog(
				FString::Format(TEXT("Prefix {0} already exists in asset name: {1}"), {*PrefixFound, OldName}));
			continue;
		}
		SpecialProcessAssetName(SelectedAsset, OldName);
		const FString NewName = *PrefixFound + OldName;
		UEditorUtilityLibrary::RenameAsset(SelectedAsset, NewName);
		++CounterOfSucceededAdding;
	}
	if (CounterOfSucceededAdding > 0)
	{
		ShowNotifyInfo(TEXT("Successfully renamed ") + FString::FromInt(CounterOfSucceededAdding) + TEXT(" assets"));
	}
}

void UQuickAssetActionUtility::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;
	FixUpRedirectors();

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		TArray<FString> AssetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(
			SelectedAssetData.GetObjectPathString());
		if (AssetReferences.IsEmpty())
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}
	if (UnusedAssetsData.IsEmpty())
	{
		ShowMessageDialog(TEXT("Failed to find unused assets"), false);
		return;
	}
	const int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);
	if (NumOfAssetsDeleted <= 0) return;
	ShowNotifyInfo(TEXT("Successfully deleted unused ") + FString::FromInt(NumOfAssetsDeleted) + TEXT(" assets"));
}

void UQuickAssetActionUtility::SpecialProcessAssetName(const UObject* Asset, FString& OldAssetName)
{
	if (Asset == nullptr) return;
	if (Asset->IsA<UMaterialInstanceConstant>())
	{
		OldAssetName.RemoveFromStart(TEXT("M_"));
		OldAssetName.RemoveFromEnd(TEXT("_inst"));
	}
}
