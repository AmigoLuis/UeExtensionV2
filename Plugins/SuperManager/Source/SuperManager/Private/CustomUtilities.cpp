#include "CustomUtilities.h"

#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"


TArray<TSharedPtr<FAssetData>> FilteredOutDeletedAssetsData(
	const TArray<TSharedPtr<FAssetData>>& AssetsDataToDelete, const int32 DeletedAssetsNum)
{
	TArray<TSharedPtr<FAssetData>> AssetsDataAlreadyDeleted;
	const FAssetRegistryModule* AssetRegistryModulePtr = LoadModulePtrWithLog<
		FAssetRegistryModule>(TEXT("AssetRegistry"));
	if (!AssetRegistryModulePtr) return AssetsDataAlreadyDeleted;
	TArray<FName> AssetsNameNotDeleted, AssetsNameAlreadyDeleted;
	FAssetData EmptyAssetData;
	for (const TSharedPtr<FAssetData>& AssetDataToDelete : AssetsDataToDelete)
	{
		if (AssetsDataToDelete.Num() == DeletedAssetsNum + AssetsNameNotDeleted.Num())
		{
			// 全部资产已删除
			AssetsNameAlreadyDeleted.Add(AssetDataToDelete->AssetName);
			AssetsDataAlreadyDeleted.Add(AssetDataToDelete);
			continue;
		}
		// 这个用于判断资产是否存在的函数没有经过测试（TryGetAssetByObjectPath）
		if (AssetRegistryModulePtr->TryGetAssetByObjectPath(AssetDataToDelete->GetSoftObjectPath(), EmptyAssetData) !=
			UE::AssetRegistry::EExists::DoesNotExist)
		{
			// 添加到没有删除的集合中（状态不是不存在的都视为删除失败）
			AssetsNameNotDeleted.Add(AssetDataToDelete->AssetName);
		}
		else
		{
			AssetsNameAlreadyDeleted.Add(AssetDataToDelete->AssetName);
			AssetsDataAlreadyDeleted.Add(AssetDataToDelete);
		}
	}

	// 低级别日志记录哪些删除成功
	for (FName AssetNameAlreadyDeleted : AssetsNameAlreadyDeleted)
	{
		PrintInLog(TEXT("Asset: \"") + AssetNameAlreadyDeleted.ToString() + TEXT("\" is deleted."),
				   SuperManager::Display);
	}
	// 如果没有全部删除，就展示哪些没有删除成功
	if (!AssetsNameNotDeleted.IsEmpty())
	{
		FString DeleteAssetsInfo(TEXT("Failed to delete "));
		DeleteAssetsInfo.Append(FString::FromInt(AssetsNameNotDeleted.Num())).Append(
			TEXT(" Assets, Listed below:\n"));
		for (FName AssetDataNotDeleted : AssetsNameNotDeleted)
		{
			DeleteAssetsInfo.Append(AssetDataNotDeleted.ToString()).Append(TEXT("\n"));
		}
		// 如果没有全部删除，就是警告级别
		ShowMessageDialog(DeleteAssetsInfo, true);
	}
	return AssetsDataAlreadyDeleted;
} 

void FixUpRedirectors()
{
	const FAssetRegistryModule* AssetRegistryModulePtr = LoadModulePtrWithLog<
		FAssetRegistryModule>(TEXT("AssetRegistry"));
	if (!AssetRegistryModulePtr) return;

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace(TEXT("/Game"));
	Filter.ClassPaths.Emplace(UObjectRedirector::StaticClass()->GetClassPathName());

	TArray<FAssetData> OutRedirectors;
	AssetRegistryModulePtr->Get().GetAssets(Filter, OutRedirectors);

	TArray<UObjectRedirector*> RedirectorsToFix;
	for (const FAssetData& RedirectorAssetData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorAssetData.GetAsset()))
		{
			RedirectorsToFix.Add(RedirectorToFix);
		}
	}
	const FAssetToolsModule* AssetToolsModule = LoadModulePtrWithLog<FAssetToolsModule>(TEXT("AssetTools"));
	if (!AssetToolsModule) return;
	AssetToolsModule->Get().FixupReferencers(RedirectorsToFix);
}

void SyncToAssetInContentBrowser(const FAssetData& AssetData)
{
	FString AssetObjectPath = AssetData.GetObjectPathString();
	UEditorAssetLibrary::SyncBrowserToObjects({AssetObjectPath});
	PrintInLog(TEXT("ContentBrowser is sync to: ") + AssetObjectPath, SuperManager::Verbose);
}