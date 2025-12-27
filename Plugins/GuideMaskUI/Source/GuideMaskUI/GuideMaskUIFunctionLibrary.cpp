// Fill out your copyright notice in the Description page of Project Settings.


#include "GuideMaskUIFunctionLibrary.h"

#include "../GuideMaskUI/UI/GuideMaskRegister.h"
#include "../GuideMaskUI/UI/GuideLayerBase.h"
#include "../GuideMaskUI/GuideMaskSettings.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


void UGuideMaskUIFunctionLibrary::ShowGuideWidget(const UObject* WorldContextObject, UWidget* InTagWidget)
{
	if (nullptr == WorldContextObject)
	{
		return;
	}

	const UGuideMaskSettings* Settings = GetDefault<UGuideMaskSettings>();
	if (ensureAlways(Settings) && Settings->DefaultLayer.ToSoftObjectPath().IsValid())
	{
		TSubclassOf<UGuideLayerBase> WidgetClass = Settings->DefaultLayer.LoadSynchronous();
		UGuideLayerBase* GuideLayer = CreateWidget<UGuideLayerBase>(WorldContextObject->GetWorld(), WidgetClass);
		
		GuideLayer->AddToViewport(0);

		if (ensure(GuideLayer))
		{
			GuideLayer->SetGuide(InTagWidget);
		}
	}
}

void UGuideMaskUIFunctionLibrary::ShowGuideListEntry(const UObject* WorldContextObject, UWidget* InTagWidget, UObject* InListItem)
{


}

void UGuideMaskUIFunctionLibrary::GetAllGuideRegisters(const UObject* WorldContextObject, TArray<UGuideMaskRegister*>& FoundWidgets)
{
	FoundWidgets.Empty();

	if (!WorldContextObject)
	{
		return;
	}

	const UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	for (TObjectIterator<UGuideMaskRegister> Itr; Itr; ++Itr)
	{
		UGuideMaskRegister* LiveWidget = *Itr;

		// Skip any widget that's not in the current world context or that is not a child of the class specified.
		if (LiveWidget->GetWorld() != World)
		{
			continue;
		}

		FoundWidgets.Add(LiveWidget);
	}
}

UWidget* UGuideMaskUIFunctionLibrary::GetTagWidget(const UObject* WorldContextObject, const FName& InTag, int InLevel)
{
	TArray<UGuideMaskRegister*> Widgets;
	GetAllGuideRegisters(WorldContextObject, OUT Widgets);

	for (UGuideMaskRegister* Register : Widgets)
	{
		if (false == Register->IsContains(InTag))
		{
			continue;
		}

		return Register->GetTagWidget(InTag, InLevel);
	}


	return nullptr;
}
