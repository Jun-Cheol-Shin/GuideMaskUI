// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "GuideActionable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGuideActionable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GUIDEMASKUI_API IGuideActionable
{
	GENERATED_BODY()

public:
	virtual void NativeOnShow();

	virtual void NativeOnAction();


	UFUNCTION(BlueprintImplementableEvent, Category = GuideActionable, meta = (DisplayName = "On Show"))
	void BP_OnShow();

	UFUNCTION(BlueprintImplementableEvent, Category = GuideActionable, meta = (DisplayName = "On Action"))
	void BP_OnAction();

};
