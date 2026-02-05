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

	// 是否启用插件
	UPROPERTY(Config, EditAnywhere, Category = "General")
	bool bEnablePlugin;

	// 默认蓝图名称
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules")
	FString DefaultBlueprintName;

	// 是否使用项目前缀
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules")
	bool bUseProjectPrefix;

	// 项目前缀
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules", meta = (EditCondition = "bUseProjectPrefix"))
	FString ProjectPrefix;

	// 是否添加时间戳
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules")
	bool bUseTimestamp;

	// 命名模式
	UPROPERTY(Config, EditAnywhere, Category = "Naming Rules")
	FString NamingPattern;

	// 是否显示通知
	UPROPERTY(Config, EditAnywhere, Category = "Notifications")
	bool bShowRenameNotification;

	// 自动重命名现有NewBlueprint资产
	UPROPERTY(Config, EditAnywhere, Category = "Advanced")
	bool bAutoRenameExisting;

	// 排除的父类
	UPROPERTY(Config, EditAnywhere, Category = "Advanced")
	TArray<FString> ExcludedParentClasses;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
