#pragma once

class FBlueprintAssetRenameHandler
{
public:
	static bool ProcessAssetIfIsBlueprint(const FAssetData& AssetData);
	// 蓝图分析功能
	static void AnalyzeAllBlueprintsInPath(const FString& Path);

	static bool IsBlueprintAsset(const FAssetData& AssetData);
	static void AnalyzeParentClass(UBlueprint* Blueprint, UClass* ParentClass);
	static bool IsBlueprintGeneratedClass(UClass* Class);
private:
	// 蓝图信息结构
	struct FBlueprintInfo
	{
		FString Name;
		FString Path;
		FString ParentClassName;
		bool bIsBlueprintParent;
		FDateTime CreatedTime;
		FDateTime ModifiedTime;
	};
	// 缓存已分析的蓝图
	TMap<FString, TSharedPtr<FBlueprintInfo>> BlueprintCache;
};
