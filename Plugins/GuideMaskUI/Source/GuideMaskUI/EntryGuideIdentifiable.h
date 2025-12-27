// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EntryGuideIdentifiable.generated.h"


class UUserWidget;
class UWidget;


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEntryGuideIdentifiable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GUIDEMASKUI_API IEntryGuideIdentifiable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCosmetic, meta = (Category = "Guide Mask UI Plugin", DisplayName = "On Get Desired Nested Widgets"))
	void GetDesiredNestedWidgets(TArray<UWidget*>& OutParam);
	virtual void GetDesiredNestedWidgets_Implementation(TArray<UWidget*>& OutParam) const {};

};
