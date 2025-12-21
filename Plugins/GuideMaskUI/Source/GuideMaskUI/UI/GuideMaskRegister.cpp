// Fill out your copyright notice in the Description page of Project Settings.


#include "GuideMaskRegister.h"
#include "Components/PanelSlot.h"
#include "Components/OverlaySlot.h"

#include "Components/TreeView.h"
#include "Components/ListView.h"
#include "Components/DynamicEntryBox.h"
#include "Components/WrapBox.h"

#define LOCTEXT_NAMESPACE "GuideMaskRegister"

#if WITH_EDITOR
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#include "Editor/WidgetCompilerLog.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "../GuideMaskSettings.h"

void UGuideMaskRegister::HidePreviewDebug()
{
	if (nullptr != LayerContent)
	{
		if (Overlay)
		{
			Overlay->RemoveSlot(LayerContent->TakeWidget());
		}

		LayerContent->RemoveFromParent();
		LayerContent = nullptr;
	}
}

void UGuideMaskRegister::ShowPreviewDebug()
{
	HidePreviewDebug();

	ForceLayoutPrepass();

	const UGuideMaskSettings* Settings = GetDefault<UGuideMaskSettings>();
	if (ensureAlways(Settings))
	{
		if (Settings->DefaultLayer.ToSoftObjectPath().IsValid())
		{
			FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
			TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(Settings->DefaultLayer.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
				[this]()
				{
					CreatePreviewLayer();
				}));
		}
	}
}

void UGuideMaskRegister::CreatePreviewLayer()
{
	const UGuideMaskSettings* Settings = GetDefault<UGuideMaskSettings>();
	if (!ensureAlways(Settings))
	{
		return;
	}

	if (UGuideLayerBase* Layer = CreateWidget<UGuideLayerBase>(GetWorld(), Settings->DefaultLayer.Get()))
	{
		UWidget* ContentWidget = GetContent();
		if (ContentWidget && 
			ContentWidget->GetCachedWidget())
		{
			if (TagWidgetList.Contains(PreviewWidgetTag))
			{
				UWidget* FoundWidget = TagWidgetList[PreviewWidgetTag];

				if (FoundWidget)
				{
					SetLayer(Layer);
					Layer->SetGuide(ContentWidget->GetCachedWidget()->GetTickSpaceGeometry(), FoundWidget);
				}
			}
		}
	}
}


TArray<FName> UGuideMaskRegister::GetTagOptions() const
{
	TArray<FName> TagList;

	Algo::Transform(TagWidgetList, TagList, [](const TPair<FName, UWidget*>& InEntry) -> FName
		{
			return InEntry.Key;
		});

	return TagList;
}

const FText UGuideMaskRegister::GetPaletteCategory()
{
	return NSLOCTEXT("UMG", "Guide Mask Plugin", "Guide Mask Plugin");
}


void UGuideMaskRegister::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);


	for (auto& [Tag, Widget] : TagWidgetList)
	{
		if (UTreeView* TreeView = Cast<UTreeView>(Widget))
		{
			TreeView->GetEntryWidgetClass();
		}

		else if (UListView* ListView = Cast<UListView>(Widget))
		{
			if (ListView->GetEntryWidgetClass() && ListView->GetEntryWidgetClass()->GetDefaultObject())
			{
				if (ListView->GetEntryWidgetClass()->ImplementsInterface(UUserObjectListEntry::StaticClass()))
				{
					//ListView->GetItem
				}
			}
 		}

		else if (UDynamicEntryBox* DynamicEntryBox = Cast<UDynamicEntryBox>(Widget))
		{

		}
	}
	
	/*
	if (UWidgetBlueprint* WidgetBlueprint = GetTypedOuter<UWidgetBlueprint>())
	{
		if (WidgetBlueprint->GeneratedClass)
		{
			if (false == WidgetBlueprint->GeneratedClass->ImplementsInterface(UGuideMaskable::StaticClass()))
			{
				CompileLog.Error(LOCTEXT("GuideMaskRegister", "Not Implemented UI Guide Maskable Interface!"));
			}
		}
	}
	*/



}
#endif


void UGuideMaskRegister::SetLayer(UWidget* InLayer)
{
	if (!ensureAlways(InLayer && Overlay))
	{
		return;
	}

	LayerContent = InLayer;
	
	if (Overlay)
	{
		Overlay->
			AddSlot()
			[
				LayerContent->TakeWidget()
			];
	}
}

TSharedRef<SWidget> UGuideMaskRegister::RebuildWidget()
{
	Overlay = SNew(SOverlay);

	if (UWidget* WidgetInDesigner = GetContent())
	{
		Overlay->
			AddSlot()
			[
				WidgetInDesigner->TakeWidget()
			];
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	return Overlay.ToSharedRef();
}

void UGuideMaskRegister::ReleaseSlateResources(bool bReleaseChildren)
{
	if (Overlay)
	{
		if (LayerContent)
		{
			Overlay->RemoveSlot(LayerContent->TakeWidget());
		}

		if (UWidget* ContentWidget = GetContent())
		{
			Overlay->RemoveSlot(ContentWidget->TakeWidget());
		}

		LayerContent = nullptr;
		Overlay = nullptr;
	}

	Super::ReleaseSlateResources(bReleaseChildren);
}

void UGuideMaskRegister::SynchronizeProperties()
{
	Super::SynchronizeProperties();


}


#undef LOCTEXT_NAMESPACE