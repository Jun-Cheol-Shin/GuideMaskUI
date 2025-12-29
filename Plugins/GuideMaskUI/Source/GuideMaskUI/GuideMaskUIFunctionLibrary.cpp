// Fill out your copyright notice in the Description page of Project Settings.


#include "GuideMaskUIFunctionLibrary.h"
#include "GuideListEntryAsyncAction.h"

#include "../GuideMaskUI/UI/GuideMaskRegister.h"
#include "../GuideMaskUI/UI/GuideLayerBase.h"
#include "../GuideMaskUI/GuideMaskSettings.h"
#include "../GuideMaskUI/EntryGuideIdentifiable.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "Components/ListView.h"
#include "Components/DynamicEntryBox.h"

void UGuideMaskUIFunctionLibrary::ShowGuideWidget(const UObject* WorldContextObject, UWidget* InTagWidget, const FGuideBoxActionParameters& InActionParam, int InLayerZOrder)
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
		
		GuideLayer->AddToViewport(InLayerZOrder);

		if (ensure(GuideLayer))
		{
			GuideLayer->SetGuide(InTagWidget, InActionParam);
		}
	}
}

void UGuideMaskUIFunctionLibrary::ShowGuideListEntry(const UObject* WorldContextObject, UListView* InTagListView, UObject* InListItem, const FGuideBoxActionParameters& InActionParam, int InLayerZOrder, float InAsyncTimeout)
{
	if (nullptr == WorldContextObject)
	{
		return;
	}

	if (UGuideListEntryAsyncAction* AsyncAction = 
		UGuideListEntryAsyncAction::Create(WorldContextObject->GetWorld(), 
			InTagListView, 
			InListItem, 
			InAsyncTimeout))
	{
		AsyncAction->OnReadyNative.AddWeakLambda(WorldContextObject,
			[InActionParam, InLayerZOrder](const UObject* InWorldContextObject, UUserWidget* InEntryWidget)
			{
				UGuideMaskUIFunctionLibrary::ShowGuideWidget(InWorldContextObject, InEntryWidget, InActionParam, InLayerZOrder);
			});

		AsyncAction->Activate();
	}

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

TMap<int, UWidget*> UGuideMaskUIFunctionLibrary::GetWidgetTree(const UObject* WorldContextObject, UWidget* InWidget)
{
	TMap<int, UWidget*> Retval;

	int Level = 1;
	ConstructTree(OUT Retval, WorldContextObject, InWidget, Level);

	return Retval;
}

void UGuideMaskUIFunctionLibrary::ConstructTree(OUT TMap<int, UWidget*>& OutWidgetTree, const UObject* WorldContextObject, UWidget* InWidget, int& OutKey)
{
	if (nullptr == InWidget)
	{
		return;
	}

	OutWidgetTree.Emplace(OutKey++, InWidget);

	if (UListView* ListView = Cast<UListView>(InWidget))
	{
		if (true == ListView->GetDisplayedEntryWidgets().IsEmpty())
		{
			ForeachEntryClass(OutWidgetTree, WorldContextObject, ListView->GetEntryWidgetClass(), OutKey);
		}

		else
		{
			ForeachEntry(OutWidgetTree, WorldContextObject , *ListView->GetDisplayedEntryWidgets().begin(), OutKey);
		}

	}

	else if (UDynamicEntryBox* EntryBox = Cast<UDynamicEntryBox>(InWidget))
	{
		if (true == EntryBox->GetAllEntries().IsEmpty())
		{
			ForeachEntryClass(OutWidgetTree, WorldContextObject, EntryBox->GetEntryWidgetClass(), OutKey);
		}

		else
		{
			ForeachEntry(OutWidgetTree, WorldContextObject, *EntryBox->GetAllEntries().begin(), OutKey);
		}
	}
}

void UGuideMaskUIFunctionLibrary::ForeachEntryClass(OUT TMap<int, UWidget*>& OutTree, const UObject* WorldContextObject, TSubclassOf<UUserWidget> InEntryClass, int& OutKey)
{
	if (UUserWidget* EntryWidget = CreateWidget<UUserWidget>(WorldContextObject->GetWorld(), InEntryClass))
	{
		ForeachEntry(OutTree, WorldContextObject, EntryWidget, OutKey);
	}
}

void UGuideMaskUIFunctionLibrary::ForeachEntry(OUT TMap<int, UWidget*>& OutTree, const UObject* WorldContextObject, UUserWidget* InEntry, int& OutKey)
{
	if (InEntry)
	{
		TArray<UWidget*> Childs;

		if (true == InEntry->GetClass()->ImplementsInterface(UEntryGuideIdentifiable::StaticClass()))
		{
			IEntryGuideIdentifiable::Execute_GetDesiredNestedWidgets(InEntry, OUT Childs);
		}

		else if (IEntryGuideIdentifiable* Identify = Cast<IEntryGuideIdentifiable>(InEntry))
		{
			Identify->GetDesiredNestedWidgets_Implementation(OUT Childs);
		}


		for (int i = 0; i < Childs.Num(); ++i)
		{
			ConstructTree(OutTree, WorldContextObject, Childs[i], OutKey);
		}
	}
}
