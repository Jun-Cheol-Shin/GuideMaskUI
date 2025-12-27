// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GuideMaskUIFunctionLibrary.generated.h"

/**
 * 
 */

class UGuideMaskRegister;

UCLASS()
class GUIDEMASKUI_API UGuideMaskUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Guide Mask UI Functions", meta = (WorldContext = "WorldContextObject"))
	static void ShowGuideWidget(const UObject* WorldContextObject, UWidget* InTagWidget);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Guide Mask UI Functions", meta = (WorldContext = "WorldContextObject"))
	static void ShowGuideListEntry(const UObject* WorldContextObject, UWidget* InTagWidget, UObject* InListItem);


	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Guide Mask UI Functions", meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "WidgetClass", DynamicOutputParam = "FoundWidgets"))
	static void GetAllGuideRegisters(const UObject* WorldContextObject, TArray<UGuideMaskRegister*>& FoundWidgets);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Guide Mask UI Functions", meta = (WorldContext = "WorldContextObject"))
	static UWidget* GetTagWidget(const UObject* WorldContextObject, const FName& InTag, int InLevel = 1);
	
};
