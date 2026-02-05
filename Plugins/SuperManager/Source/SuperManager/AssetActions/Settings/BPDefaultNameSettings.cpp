// Fill out your copyright notice in the Description page of Project Settings.


#include "BPDefaultNameSettings.h"


UBPDefaultNameSettings::UBPDefaultNameSettings()
{
	// 默认设置
	bEnablePlugin = true;
	DefaultBlueprintName = TEXT("NewBlueprint");
	bUseProjectPrefix = false;
	ProjectPrefix = TEXT("MyProject");
	bUseTimestamp = false;
	bShowRenameNotification = true;
	bAutoRenameExisting = false;
	NamingPattern = TEXT("{Project}_{BaseName}_{Timestamp}");
    
	// 默认排除的父类
	ExcludedParentClasses.Add(TEXT("Actor"));
	ExcludedParentClasses.Add(TEXT("Pawn"));
}

#if WITH_EDITOR
void UBPDefaultNameSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
    
	// 保存配置
	SaveConfig();
    
	// 通知模块重新加载设置
	if (FModuleManager::Get().IsModuleLoaded("BlueprintDefaultName"))
	{
		// 可以通过事件或委托通知主模块设置已更改
	}
}
#endif