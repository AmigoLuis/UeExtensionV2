// Fill out your copyright notice in the Description page of Project Settings.


#include "BPDefaultNameSettings.h"

#include "CheckAndLogAndReturn.h"
#include "PrintInLog.h"


UBPDefaultNameSettings::UBPDefaultNameSettings()
{
	// 默认设置
	bEnablePlugin = true;
	DefaultBlueprintName = TEXT("NewBlueprint");
	bUseBlueprintPrefix = true;
	BlueprintPrefix = TEXT("BP_");
	bShowRenameNotification = true;
	NamingPattern = TEXT("{Prefix}{ParentClassName}_{IntSuffix}");
	UMaxIntSuffixValue = UINT32_MAX - 1;
}


void UBPDefaultNameSettings::CopySettingsValues(const UBPDefaultNameSettings* Src)
{
	CHECK_NULL_RETURN(Src);
	bEnablePlugin = Src->bEnablePlugin;
	DefaultBlueprintName = Src->DefaultBlueprintName;
	bUseBlueprintPrefix = Src->bUseBlueprintPrefix;
	BlueprintPrefix = Src->BlueprintPrefix;
	NamingPattern = Src->NamingPattern;
	bShowRenameNotification = Src->bShowRenameNotification;
	UMaxIntSuffixValue = Src->UMaxIntSuffixValue;
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