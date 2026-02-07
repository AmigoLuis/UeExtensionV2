#pragma once

class UBPDefaultNameSettings;

class FBlueprintAssetRenameHandler
{
public:
	static void RegisterBlueprintAssetRenameHandler();
	static void UnregisterBlueprintAssetRenameHandler();
	static bool ProcessAssetIfIsBlueprint(const FAssetData& AssetData);
	// 蓝图分析功能
	static void AnalyzeAllBlueprintsInPath(const FString& Path);

	static bool IsAssetAbleToRename(const FAssetData& AssetData);
	static bool RenameAssetDerivedFromBlueprint(const FAssetData& AssetData, const FString& ParentClassName);
	static bool RenameAssetDerivedFromCPP(const FAssetData& AssetData, const FString& ParentClassName);
	static bool IsBlueprintAsset(const FAssetData& AssetData);
	static void AnalyzeParentClass(const FAssetData& AssetData, UBlueprint* Blueprint, UClass* ParentClass);
	static bool IsBlueprintGeneratedClass(UClass* Class);
private:
	// 蓝图信息结构
	static TWeakObjectPtr<UBPDefaultNameSettings> GetOrNewBPDefaultNameSettingsWeakPtr();
};
