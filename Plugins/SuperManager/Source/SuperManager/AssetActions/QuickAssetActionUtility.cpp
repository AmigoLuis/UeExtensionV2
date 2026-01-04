// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickAssetActionUtility.h"

void UQuickAssetActionUtility::TestFunc()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Green, TEXT("QuickAssetActionUtility TestFunc"));
	}
}
