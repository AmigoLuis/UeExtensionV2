// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "NiagaraEmitter.h"
#include "Blueprint/UserWidget.h"
#include "NiagaraSystem.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "QuickAssetActionUtility.generated.h"

/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickAssetActionUtility : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor, Category = "SuperManager")
	void DuplicateAssets(int32 NumOfDuplicates);
	UFUNCTION(CallInEditor, Category = "SuperManager")
	void AutoAddPrefixes();

private:
	/**
	 * 
	 */
	TMap<UClass*, FString> AssetTypeToPrefixMapping = {
		{UBlueprint::StaticClass(), TEXT("BP_")}, // 蓝图
		{UStaticMesh::StaticClass(), TEXT("SM_")}, // 静态网格体
		{USkeletalMesh::StaticClass(), TEXT("SK_")}, // 骨骼网格体
		{UMaterial::StaticClass(), TEXT("M_")}, // 材质类
		{UPhysicalMaterial::StaticClass(), TEXT("PM_")}, // 物理材质
		{UMaterialInstanceConstant::StaticClass(), TEXT("MI_")}, // 材质实例类
		{UMaterialFunction::StaticClass(), TEXT("MF_")}, // 材质函数类
		{UTexture::StaticClass(), TEXT("T_")}, // 纹理
		{UAnimSequence::StaticClass(), TEXT("A_")}, // 动画序列
		{UAnimBlueprint::StaticClass(), TEXT("ABP_")}, // 动画蓝图
		{UParticleSystem::StaticClass(), TEXT("FX_")}, // 粒子系统
		{USoundWave::StaticClass(), TEXT("SFX_")}, // 声音波形
		{USoundCue::StaticClass(), TEXT("SND_")}, // 声音Cue
		{UUserWidget::StaticClass(), TEXT("WBP_")}, // 小部件蓝图
		{UParticleSystem::StaticClass(), TEXT("FX_")}, // 粒子系统
		{UNiagaraSystem::StaticClass(), TEXT("NS_")}, // 
		{UNiagaraEmitter::StaticClass(), TEXT("NS_")}, // 
		{UDataTable::StaticClass(), TEXT("DT_")} // 数据表
	};
};
