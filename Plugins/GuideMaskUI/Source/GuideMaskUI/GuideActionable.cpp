// Fill out your copyright notice in the Description page of Project Settings.


#include "GuideActionable.h"


// Add default functionality here for any IGuideActionable functions that are not pure virtual.

void IGuideActionable::NativeOnShow()
{
	Execute_BP_OnShow(Cast<UObject>(this));
}

void IGuideActionable::NativeOnAction()
{
	Execute_BP_OnAction(Cast<UObject>(this));
}
