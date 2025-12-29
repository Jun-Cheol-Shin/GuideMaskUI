// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GuideListEntryAsyncAction.generated.h"

class UListView;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnListEntryReadyEvent, const UObject*, InWorldContextObject, UUserWidget*, EntryWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnListEntryFailedEvent);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListEntryReadyNativeEvent, const UObject*, UUserWidget*);
DECLARE_MULTICAST_DELEGATE(FOnListEntryFailedNativeEvent);

UCLASS()
class GUIDEMASKUI_API UGuideListEntryAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:


	UPROPERTY(BlueprintAssignable) 
	FOnListEntryReadyEvent OnReady;
	FOnListEntryReadyNativeEvent OnReadyNative;

	UPROPERTY(BlueprintAssignable) 
	FOnListEntryFailedEvent OnFailed;
	FOnListEntryFailedNativeEvent OnFailedNative;


	UFUNCTION(BlueprintCallable, Category = "Guide", meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject", DisplayName = "Wait Guide List Entry"))
	static UGuideListEntryAsyncAction* Create(UObject* InWorldContextObject, 
		UListView* InListView, 
		UObject* InListItem, 
		float InTimeout);

	virtual void Activate() override;

private:
	void HandleItemScrolledIntoView(UObject* Item, UUserWidget& EntryWidget);

private:
	bool Tick(float DeltaSeconds);
	void Success(UUserWidget* EntryWidget);
	void Fail();
	void Clear();
	
private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContext;

	UPROPERTY()
	TObjectPtr<UListView> ListViewPtr;

	UPROPERTY()
	TObjectPtr<UObject> ItemPtr;

	bool bFindEntry = false;

	FTSTicker::FDelegateHandle TickerHandle;
	double StartTime = 0.0;
	float Timeout = 3.f;


	//bool bDoScroll = true;
};
