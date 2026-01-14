// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickMaterialCreation.h"

#include "DebugHeader.h"

void UQuickMaterialCreation::CreateMaterialFromSelectedTextures()
{
	LOG_ENTER_FUNCTION();
	if (bUseCustomMaterialName)
	{
		if (MaterialNam.IsEmpty() || MaterialNam.Equals(DefaultMaterialName))
		{
			ShowMessageDialog("Please input a valid material name");
		}
	}
}
