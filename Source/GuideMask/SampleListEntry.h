// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "GuideMaskUI/GuideActionable.h"
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
class GUIDEMASK_API USampleListEntry : public UUserWidget, public IUserObjectListEntry, public IGuideActionable
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	virtual void NativeOnShow() override;
	virtual void NativeOnAction() override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UDynamicEntryBox* AdditionalEntryBox;
};
