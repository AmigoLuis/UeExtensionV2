// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BPDefaultNameSettings.generated.h"

/**
 * 
 */
UCLASS(config=Editor, defaultconfig)
class SUPERMANAGER_API UBPDefaultNameSettings : public UObject
{
	GENERATED_BODY()

public:
	UBPDefaultNameSettings();

	void CopySettingsValues(const UBPDefaultNameSettings* Src);
	// 是否启用插件
	UPROPERTY(Config, EditAnywhere, Category = "General")
	bool bEnablePlugin;

	// 默认蓝图名称
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules")
	FString DefaultBlueprintName;

	// 是否使用蓝图前缀
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules")
	bool bUseBlueprintPrefix;

	// 蓝图前缀，一般是BP_
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules", meta = (EditCondition = "bUseBlueprintPrefix"))
	FString BlueprintPrefix;

	// 最大的整数后缀（重命名时如果模板里面有IntSuffix，存在重名资产时默认IntSuffix+1再尝试重命名，达到UMaxIntSuffixValue时重命名失败）
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules")
	uint32 UMaxIntSuffixValue;
	
	// 命名模式
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules")
	FString NamingPattern;

	// 是否显示通知
	UPROPERTY(Config, EditAnywhere, Category = "Notifications")
	bool bShowRenameNotification;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
