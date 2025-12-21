// Fill out your copyright notice in the Description page of Project Settings.


#include "GuideMaskSettings.h"

UGuideMaskSettings::UGuideMaskSettings(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//virtual FName GetContainerName() const override { return TEXT("Project"); }
	//virtual FName GetCategoryName()  const override { return TEXT("Plugins"); }
	//virtual FName GetSectionName()   const override { return TEXT("Guide Mask Settings"); }

	CategoryName = "Plugins";
	SectionName = "Guide Mask Settings";
}
