// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FSuperManagerStyle
{
public:
	static void InitializeIcons();
	static void DeInitializeIcons();
	static FName GetStyleSetName() { return StyleSetName; }
	static FName GetDeleteUnusedAssetsIconName() { return DeleteUnusedAssetsIconName; }
	static FName GetDeleteEmptyFoldersIconName() { return DeleteEmptyFoldersIconName; }
	static FName GetAdvancedDeletionIconName() { return AdvancedDeletionIconName; }
private:
	static FName StyleSetName;
	static FName DeleteUnusedAssetsIconName;
	static FName DeleteEmptyFoldersIconName;
	static FName AdvancedDeletionIconName;
	static TSharedPtr<FSlateStyleSet> CreateSlateStyleSet();
	static TSharedPtr<FSlateStyleSet> CreatedSlateStyleSet;
};
