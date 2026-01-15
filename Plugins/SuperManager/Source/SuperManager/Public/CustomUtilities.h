#pragma once
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

inline FString GetModuleLoadFailureReason(const EModuleLoadResult Result)
{
	switch (Result)
	{
	case EModuleLoadResult::Success: return TEXT("Module loaded successfully.");
	case EModuleLoadResult::FileNotFound: return TEXT("The specified module file could not be found.");
	case EModuleLoadResult::FileIncompatible: return TEXT(
			"The specified module file is incompatible with the module system.");
	case EModuleLoadResult::CouldNotBeLoadedByOS: return TEXT("The operating system failed to load the module file.");
	case EModuleLoadResult::FailedToInitialize: return TEXT("Module initialization failed.");
	case EModuleLoadResult::NotLoadedByGameThread: return TEXT(
			"A thread attempted to load the module before the Game thread did.");
	default: return TEXT("Unknown error");
	}
}

template <typename T>
FString GetTypeNameInTemplate()
{
#ifdef _MSC_VER
	FString FuncSig = ANSI_TO_TCHAR(__FUNCSIG__);

	// 查找 "GetTypeNameInTemplate<" 的位置
	if (int32 StartPos = FuncSig.Find(TEXT("GetTypeNameInTemplate<")); StartPos != INDEX_NONE)
	{
		StartPos += 22; // 跳过 "GetTypeNameInTemplate<"

		// 查找对应的'>'
		int32 Depth = 1;
		int32 CurrentPos = StartPos;
		int32 EndPos = INDEX_NONE;

		for (; CurrentPos < FuncSig.Len(); ++CurrentPos)
		{
			if (const TCHAR Ch = FuncSig[CurrentPos]; Ch == TEXT('<'))
				Depth++;
			else if (Ch == TEXT('>'))
			{
				Depth--;
				if (Depth == 0)
				{
					EndPos = CurrentPos;
					break;
				}
			}
		}

		if (EndPos != INDEX_NONE)
		{
			return FuncSig.Mid(StartPos, EndPos - StartPos);
		}
	}
#elif
	return "only supported in _MSC_VER.";
#endif
	return "unknown error.";
}

template <typename TModuleInterface>
TModuleInterface* LoadModulePtrWithLog(const FName InModuleName)
{
	EModuleLoadResult OutFailureReason;
	TModuleInterface* AssetRegistryModulePtr = static_cast<TModuleInterface*>(
		FModuleManager::Get().LoadModuleWithFailureReason(InModuleName, OutFailureReason));
	if (!AssetRegistryModulePtr)
	{
		PrintInLog(
			FString::Format(
				TEXT("Failed to load module. module name: {0} ,module ptr type: {1}. Failure Reason: {3}."),
				{
					*InModuleName.ToString(), GetTypeNameInTemplate<TModuleInterface>(),
					GetModuleLoadFailureReason(OutFailureReason)
				}),
				SuperManager::ELogLevel::Fatal);
	}
	else
	{
		PrintInLog(
			FString::Format(
				TEXT("Successfully loaded module. module name: {0} ,module ptr type: {1}."), {
					*InModuleName.ToString(), GetTypeNameInTemplate<TModuleInterface>()
				}),
				SuperManager::ELogLevel::Display);
	}
	return AssetRegistryModulePtr;
}

void FixUpRedirectors();

inline bool IsUnrealProtectedPath(const FString& Path)
{
	return Path.Contains("Developers") 
			|| Path.Contains("Collections")
			|| Path.Contains("__ExternalActors__")
			|| Path.Contains("__ExternalObjects__");
}
TArray<TSharedPtr<FAssetData>> FilteredOutDeletedAssetsData(
	const TArray<TSharedPtr<FAssetData>>& AssetsDataToDelete, const int32 DeletedAssetsNum);

// 过滤出不存在包引用的资产
inline void FilteredOutUnusedAssetsData(
	const TArray<TSharedPtr<FAssetData>>& AllAssetsDataToFilter, 
	TArray<TSharedPtr<FAssetData>>& FilterOutUnusedAssetsData)
{
	if (AllAssetsDataToFilter.IsEmpty()) return;
	FilterOutUnusedAssetsData.Empty();
	for (TSharedPtr AssetDataToFilter : AllAssetsDataToFilter)
	{
		// 过滤出不存在包引用的资产
		if (UEditorAssetLibrary::FindPackageReferencersForAsset(AssetDataToFilter->GetObjectPathString()).IsEmpty())
		{
			FilterOutUnusedAssetsData.Add(AssetDataToFilter);
		}
	}
}

// 过滤出重名的资产
inline void FilteredOutAssetsDataWithSameName(
	const TArray<TSharedPtr<FAssetData>>& AllAssetsDataToFilter, 
	TArray<TSharedPtr<FAssetData>>& FilterOutSameNameAssetsData)
{
	if (AllAssetsDataToFilter.IsEmpty()) return;
	FilterOutSameNameAssetsData.Empty();
	const TSet<FName> AddedSameAssetsName;
	TMap<FName, TSharedPtr<FAssetData>> IteratedAssetsName;
	for (const TSharedPtr AssetDataToFilter : AllAssetsDataToFilter)
	{
		const FName& AssetName = AssetDataToFilter->AssetName;
		if (const TSharedPtr<FAssetData>* Found = IteratedAssetsName.Find(AssetName))
		{
			if (!AddedSameAssetsName.Contains(AssetName))
			{
				FilterOutSameNameAssetsData.Add(*Found);
			}
			FilterOutSameNameAssetsData.Add(AssetDataToFilter);
		}
		else
		{
			IteratedAssetsName.Add(AssetDataToFilter->AssetName, AssetDataToFilter);
		}
	}
	// TMultiMap<FName, TSharedPtr<FAssetData>> NameAndAssetsData;
	// for (const TSharedPtr AssetDataToFilter : AllAssetsDataToFilter)
	// {
	// 	// 过滤出重名的资产
	// 	NameAndAssetsData.Add(AssetDataToFilter->AssetName, AssetDataToFilter);
	// }
	// NameAndAssetsData.MultiFind()
}

inline int32 DeleteAssetsAndLog(const TArray<FAssetData>& AssetsDataToDelete)
{
	const int32 DeletedAssetsNum = ObjectTools::DeleteAssets(AssetsDataToDelete);
	PrintInLog(TEXT("Deleted ") + FString::FromInt(DeletedAssetsNum) + TEXT(" Assets."),
				SuperManager::Display);
	FixUpRedirectors();
	return DeletedAssetsNum;
} 

inline int32 DeleteAssetsAndLog(const FAssetData& AssetDataToDelete)
{
	const int32 DeletedAssetsNum =  DeleteAssetsAndLog(TArray{AssetDataToDelete});
	if (DeletedAssetsNum > 0)
	{
		PrintInLog(AssetDataToDelete.AssetName.ToString() + TEXT(" is deleted."),
SuperManager::Display);
	}
	else
	{
		PrintInLog(TEXT("Failed to delete asset :") + AssetDataToDelete.AssetName.ToString() + TEXT("."),
SuperManager::Error);
	}
	return DeletedAssetsNum;
} 

void SyncToAssetInContentBrowser(const FAssetData& AssetData);

FString GetAssetPrefixByAssetClass(const UClass* AssetClass);

bool CheckIfAssetNameExistsInFolder(const FString& FolderPath, const FString& AssetName);