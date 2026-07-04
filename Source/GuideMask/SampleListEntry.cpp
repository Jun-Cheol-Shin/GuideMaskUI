// Fill out your copyright notice in the Description page of Project Settings.


#include "SampleListEntry.h"
#include "SampleDynamicEntry.h"

#include "Components/DynamicEntryBox.h"

void USampleListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (nullptr != AdditionalEntryBox)
	{
		AdditionalEntryBox->Reset();
		for (int i = 0; i < 3; ++i)
		{
			AdditionalEntryBox->CreateEntry<USampleDynamicEntry>();
		}
	}

}

void USampleListEntry::NativeOnShow()
{
	UE_LOG(LogTemp, Warning, TEXT("On Show!! %s"), ANSI_TO_TCHAR(__FUNCTION__));
}

void USampleListEntry::NativeOnAction()
{
	UE_LOG(LogTemp, Warning, TEXT("On Action!! %s"), ANSI_TO_TCHAR(__FUNCTION__));
}
