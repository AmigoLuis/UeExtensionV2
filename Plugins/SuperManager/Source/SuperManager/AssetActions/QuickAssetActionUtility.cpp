// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickAssetActionUtility.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

void UQuickAssetActionUtility::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		auto UserChoice = ShowMessageDialog(TEXT("Please provide a valid number of duplicates as parameter."), EAppMsgType::Ok);
		if (UserChoice != EAppReturnType::Yes)
		{
			
		}
		// PrintInLog(TEXT("Please provide a valid number of duplicates as parameter."));
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
		ShowNotifyInfo(TEXT("Successfully duplicated ") + FString::FromInt(CounterOfSucceededDuplication) + TEXT(" files"));
		PrintInLog(TEXT("Successfully duplicated ") + FString::FromInt(CounterOfSucceededDuplication) + TEXT(" files"));	
	}
}
