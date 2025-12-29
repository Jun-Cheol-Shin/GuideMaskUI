// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GuideMaskUI/UI/GuideBoxBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GuideMaskUIFunctionLibrary.generated.h"

/**
 * 
 */

class UGuideMaskRegister;
class UListView;

UCLASS()
class GUIDEMASKUI_API UGuideMaskUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Guide Mask UI Functions", meta = (WorldContext = "WorldContextObject"))
	static void ShowGuideWidget(const UObject* WorldContextObject, UWidget* InTagWidget, const FGuideBoxActionParameters& InActionParam = FGuideBoxActionParameters(), int InLayerZOrder = 0);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Guide Mask UI Functions", meta = (WorldContext = "WorldContextObject"))
	static void ShowGuideListEntry(const UObject* WorldContextObject, UListView* InTagListView, UObject* InListItem, const FGuideBoxActionParameters& InActionParam = FGuideBoxActionParameters(), int InLayerZOrder = 0, float InAsyncTimeout = 1.f);


	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Guide Mask UI Functions", meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "WidgetClass", DynamicOutputParam = "FoundWidgets"))
	static void GetAllGuideRegisters(const UObject* WorldContextObject, TArray<UGuideMaskRegister*>& FoundWidgets);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Guide Mask UI Functions", meta = (WorldContext = "WorldContextObject"))
	static UWidget* GetTagWidget(const UObject* WorldContextObject, const FName& InTag, int InLevel = 1);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Guide Mask UI Functions", meta = (WorldContext = "WorldContextObject"))
	static TMap<int, UWidget*> GetWidgetTree(const UObject* WorldContextObject, UWidget* InWidget);

	
private:
	static void ConstructTree(OUT TMap<int, UWidget*>& OutWidgetTree, const UObject* WorldContextObject, UWidget* InWidget, int& OutKey);
	static void ForeachEntryClass(OUT TMap<int, UWidget*>& OutTree, const UObject* WorldContextObject, TSubclassOf<UUserWidget> InEntryClass, int& OutKey);
	static void ForeachEntry(OUT TMap<int, UWidget*>& OutTree, const UObject* WorldContextObject, UUserWidget* InEntry, int& OutKey);
};
