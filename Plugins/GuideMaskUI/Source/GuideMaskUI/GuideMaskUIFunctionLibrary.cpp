// Fill out your copyright notice in the Description page of Project Settings.


#include "GuideMaskUIFunctionLibrary.h"
#include "GuideListEntryAsyncAction.h"

#include "../GuideMaskUI/UI/GuideMaskRegister.h"
#include "../GuideMaskUI/UI/GuideLayerBase.h"
#include "../GuideMaskUI/GuideMaskSettings.h"
#include "../GuideMaskUI/NestedWidgetProvidable.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "Components/ListView.h"
#include "Components/DynamicEntryBox.h"

void UGuideMaskUIFunctionLibrary::ShowGuide(UObject* WorldContextObject, FName InTag, const FGuideBoxActionParameters& InActionParam, const TArray<FGuideDynamicWidgetPath>& InPath, int InLayerZOrder, float InAsyncTimeout)
{
	if (nullptr == WorldContextObject)
	{
		return;
	}

	UWidget* SourceWidget = GetTagWidget(WorldContextObject, InTag);
	if (!ensureMsgf(SourceWidget, TEXT("Invalid Source Widget!! %s"), ANSI_TO_TCHAR(__FUNCTION__)))
	{
		return;
	}

	ShowGuideInternal(WorldContextObject, SourceWidget, InTag, InActionParam, InPath, InLayerZOrder, InAsyncTimeout);
}

void UGuideMaskUIFunctionLibrary::ShowGuideInternal(UObject* WorldContextObject, UWidget* InWidget, FName InTag, const FGuideBoxActionParameters& InActionParam, const TArray<FGuideDynamicWidgetPath>& InPath, int InLayerZOrder, float InAsyncTimeout)
{
	if (nullptr == WorldContextObject)
	{
		return;
	}

#if ENGINE_MAJOR_VERSION >= 5
	if (true == InPath.IsEmpty())
	{
		ShowLayer(WorldContextObject, InWidget, InTag, InActionParam, InLayerZOrder);
		return;
	}
#else
	if (0 >= InPath.Num())
	{
		ShowGuideWidget(WorldContextObject, InWidget, InTag, InActionParam, InLayerZOrder);
		return;
	}
#endif

	TArray<FGuideDynamicWidgetPath> NewPath;
	for (int i = 1; i < InPath.Num(); ++i)
	{
		NewPath.Emplace(InPath[i]);
	}

	FGuideDynamicWidgetPath CurrentPath = InPath[0];
	if (UListView* ListView = Cast<UListView>(InWidget))
	{
		UObject* const* ListItem = ListView->GetListItems().FindByPredicate([Event = CurrentPath.Predicate](UObject* InItem) -> bool
			{
				return true == Event.IsBound() ? Event.Execute(EGuideWidgetPredTarget::ListItem, InItem) : false;
			});

		if (ListItem && *ListItem)
		{
			if (UGuideListEntryAsyncAction* AsyncAction =
				UGuideListEntryAsyncAction::Create(WorldContextObject->GetWorld(),
					ListView,
					*ListItem,
					InAsyncTimeout))
			{
				AsyncAction->OnReadyNative.AddWeakLambda(WorldContextObject,
					[InTag, NewPath, ChildIndex = CurrentPath.NextChildIndex, InActionParam, InLayerZOrder, InAsyncTimeout](UObject* InWorldContextObject, UUserWidget* InEntryWidget)
					{
						if (nullptr == InEntryWidget)
						{
							return;
						}

						TArray<UWidget*> Childs;
						if (true == InEntryWidget->GetClass()->ImplementsInterface(UNestedWidgetProvidable::StaticClass()))
						{
							INestedWidgetProvidable::Execute_GetDesiredNestedWidgets(InEntryWidget, OUT Childs);
						}

						else if (INestedWidgetProvidable* Identify = Cast<INestedWidgetProvidable>(InEntryWidget))
						{
							Identify->GetDesiredNestedWidgets_Implementation(OUT Childs);
						}

						if (false == Childs.IsValidIndex(ChildIndex))
						{
							ShowLayer(InWorldContextObject, InEntryWidget, InTag, InActionParam, InLayerZOrder);
						}

						else
						{
							ShowGuideInternal(InWorldContextObject, Childs[ChildIndex], InTag, InActionParam, NewPath, InLayerZOrder, InAsyncTimeout);
						}
					});

				AsyncAction->OnFailedNative.AddWeakLambda(WorldContextObject,
					[InTag, WorldContextObject, ListView, InActionParam, InLayerZOrder]()
					{
						ShowLayer(WorldContextObject, ListView, InTag, InActionParam, InLayerZOrder);
					});

				AsyncAction->Activate();
			}
		}
	}

	else if (UDynamicEntryBox* EntryBox = Cast<UDynamicEntryBox>(InWidget))
	{
		UUserWidget* const* Entry = EntryBox->GetAllEntries().FindByPredicate([Event = CurrentPath.Predicate](UUserWidget* InEntry)
			{
				return true == Event.IsBound() ? Event.Execute(EGuideWidgetPredTarget::EntryWidget, InEntry) : false;
			});

		UUserWidget* EntryPtr = Entry && *Entry ? *Entry : nullptr;

		if (nullptr != EntryPtr)
		{
			TArray<UWidget*> Childs;
			if (true == EntryPtr->GetClass()->ImplementsInterface(UNestedWidgetProvidable::StaticClass()))
			{
				INestedWidgetProvidable::Execute_GetDesiredNestedWidgets(EntryPtr, OUT Childs);
			}

			else if (INestedWidgetProvidable* Identify = Cast<INestedWidgetProvidable>(EntryPtr))
			{
				Identify->GetDesiredNestedWidgets_Implementation(OUT Childs);
			}

			if (false == Childs.IsValidIndex(CurrentPath.NextChildIndex))
			{
				ShowLayer(WorldContextObject, EntryPtr, InTag, InActionParam, InLayerZOrder);
			}

			else
			{
				ShowGuideInternal(WorldContextObject, Childs[CurrentPath.NextChildIndex], InTag, InActionParam, NewPath, InLayerZOrder, InAsyncTimeout);
			}
		}

		else
		{
			ShowLayer(WorldContextObject, EntryBox, InTag, InActionParam, InLayerZOrder);
		}
	}

	else
	{
		ShowLayer(WorldContextObject, InWidget, InTag, InActionParam, InLayerZOrder);
	}
}

void UGuideMaskUIFunctionLibrary::ShowLayer(UObject* WorldContextObject, UWidget* InTagWidget, FName InTag, const FGuideBoxActionParameters& InActionParam, int InLayerZOrder)
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

		if (!ensureMsgf(GuideLayer, TEXT("Invalid Default Layer Widget. %s"), ANSI_TO_TCHAR(__FUNCTION__)))
		{
			return;
		}

		GuideLayer->AddToViewport(InLayerZOrder);
		GuideLayer->SetGuide(InTagWidget, InTag, InActionParam);
	}
}

//
//void UGuideMaskUIFunctionLibrary::ShowGuideListEntry(UObject* WorldContextObject, UListView* InTagListView, UObject* InListItem, const FGuideBoxActionParameters& InActionParam, int InLayerZOrder, float InAsyncTimeout)
//{
//	if (nullptr == WorldContextObject)
//	{
//		return;
//	}
//
//	if (UGuideListEntryAsyncAction* AsyncAction = 
//		UGuideListEntryAsyncAction::Create(WorldContextObject->GetWorld(), 
//			InTagListView, 
//			InListItem, 
//			InAsyncTimeout))
//	{
//		AsyncAction->OnReadyNative.AddWeakLambda(WorldContextObject,
//			[InActionParam, InLayerZOrder](UObject* InWorldContextObject, UUserWidget* InEntryWidget)
//			{
//				UGuideMaskUIFunctionLibrary::ShowGuideWidget(InWorldContextObject, InEntryWidget, InActionParam, InLayerZOrder);
//			});
//
//		AsyncAction->Activate();
//	}
//}
//
//


void UGuideMaskUIFunctionLibrary::GetAllGuideRegisters(UObject* WorldContextObject, TArray<UGuideMaskRegister*>& FoundWidgets)
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

UWidget* UGuideMaskUIFunctionLibrary::GetTagWidget(UObject* WorldContextObject, const FName& InTag)
{
	if (UGuideMaskRegister* Register = GetRegister(WorldContextObject, InTag))
	{
		return Register->GetTagWidget(InTag);
	}

	return nullptr;
}

UGuideMaskRegister* UGuideMaskUIFunctionLibrary::GetRegister(UObject* WorldContextObject, const FName& InTag)
{
	TArray<UGuideMaskRegister*> Widgets;
	GetAllGuideRegisters(WorldContextObject, OUT Widgets);

	UGuideMaskRegister** FoundRegister = Widgets.FindByPredicate([InTag](UGuideMaskRegister* InRegister)
		{
			return InRegister && InRegister->IsContains(InTag);
		});

	return FoundRegister && *FoundRegister ? *FoundRegister : nullptr;
}