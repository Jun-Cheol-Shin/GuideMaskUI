// Fill out your copyright notice in the Description page of Project Settings.


#include "GuideListEntryAsyncAction.h"

#include "Components/ListView.h"
#include "Components/TreeView.h"

UGuideListEntryAsyncAction* UGuideListEntryAsyncAction::Create(UObject* InWorldContextObject, UListView* InListView, UObject* InListItem, float InTimeout)
{
	UGuideListEntryAsyncAction* NewAction = NewObject<UGuideListEntryAsyncAction>();
	NewAction->WorldContext = InWorldContextObject;
	NewAction->ListViewPtr = InListView;
	NewAction->ItemPtr = InListItem;
	NewAction->Timeout = FMath::Max(0.5f, InTimeout);
	//NewAction->bDoScroll = bScrollIntoView;

	return NewAction;
}

void UGuideListEntryAsyncAction::Activate()
{
	if (nullptr == ListViewPtr || nullptr == ItemPtr)
	{
		Fail();
		return;
	}


	if (UUserWidget* EntryWidget = ListViewPtr->GetEntryWidgetFromItem(ItemPtr))
	{
		Success(EntryWidget);
	}

	else
	{
		ListViewPtr->OnItemScrolledIntoView().AddUObject(this, &UGuideListEntryAsyncAction::HandleItemScrolledIntoView);

		int Index = ListViewPtr->GetListItems().IndexOfByPredicate([&](UObject* InItem)
			{
				return InItem == ItemPtr;
			});

		if (Index != INDEX_NONE)
		{
			ListViewPtr->ScrollIndexIntoView(Index);

			StartTime = FPlatformTime::Seconds();
			FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
			TickerHandle = FTSTicker::GetCoreTicker().AddTicker(
				FTickerDelegate::CreateUObject(this, &UGuideListEntryAsyncAction::Tick));
		}

		else
		{
			Fail();
		}
	}

}

void UGuideListEntryAsyncAction::HandleItemScrolledIntoView(UObject* Item, UUserWidget& EntryWidget)
{
	if (Item == ItemPtr)
	{
		bFindEntry = true;
	}
}

bool UGuideListEntryAsyncAction::Tick(float DeltaSeconds)
{
	if (Timeout > 0.f && (FPlatformTime::Seconds() - StartTime) >= Timeout)
	{
		Fail();
		return false;
	}

	if (bFindEntry)
	{
		UUserWidget* Widget = ListViewPtr->GetEntryWidgetFromItem(ItemPtr);
		if (Widget && false == Widget->TakeWidget()->NeedsPrepass())
		{
			Success(Widget);
			return false;
		}
	}

	return true;
}

void UGuideListEntryAsyncAction::Success(UUserWidget* EntryWidget)
{
	OnReadyNative.Broadcast(WorldContext, EntryWidget);
	OnReady.Broadcast(WorldContext, EntryWidget);

	Clear();
	SetReadyToDestroy();
}

void UGuideListEntryAsyncAction::Fail()
{
	OnFailedNative.Broadcast();
	OnFailed.Broadcast();

	Clear();
	SetReadyToDestroy();
}

void UGuideListEntryAsyncAction::Clear()
{
	bFindEntry = false;

	if (ListViewPtr)
	{
		ListViewPtr->OnItemScrolledIntoView().RemoveAll(this);
	}
}