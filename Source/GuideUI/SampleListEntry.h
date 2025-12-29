// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SampleListEntry.generated.h"


class UDynamicEntryBox;

UCLASS(BlueprintType)
class USampleListItem : public UObject
{
	GENERATED_BODY()

public:
	int ItemId = 0;
};

UCLASS()
class GUIDEUI_API USampleListEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UDynamicEntryBox* AdditionalEntryBox;
};
