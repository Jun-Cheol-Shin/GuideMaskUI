// Fill out your copyright notice in the Description page of Project Settings.


#include "GuideMaskRegister.h"
#include "Components/PanelSlot.h"
#include "Components/OverlaySlot.h"

#include "Components/TreeView.h"
#include "Components/ListView.h"
#include "Components/DynamicEntryBox.h"
#include "Components/WrapBox.h"

#include "../EntryGuideIdentifiable.h"

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
	if (ensureAlways(Settings) && Settings->DefaultLayer.ToSoftObjectPath().IsValid())
	{
		UWidget* ContentWidget = GetContent();
		if (ContentWidget && ContentWidget->GetCachedWidget())
		{
			FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
			TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(Settings->DefaultLayer.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this,
				[this, Geometry = ContentWidget->GetCachedWidget()->GetTickSpaceGeometry()]()
				{
					CreatePreviewLayer(Geometry);
				}));
		}
	}
}

void UGuideMaskRegister::ConstructWidgetTree(OUT TMap<int, UWidget*>& OutTree, UWidget* InWidget, int& InKey) const
{
	if (nullptr == InWidget)
	{
		return;
	}

	OutTree.Emplace(InKey++, InWidget);

	if (UListViewBase* ListView = Cast<UListViewBase>(InWidget))
	{
		if (true == ListView->GetDisplayedEntryWidgets().IsEmpty())
		{
			ForeachEntryClass(OutTree, ListView->GetEntryWidgetClass(), InKey);
		}

		else
		{
			ForeachEntry(OutTree, *ListView->GetDisplayedEntryWidgets().begin(), InKey);
		}

	}

	else if (UDynamicEntryBox* EntryBox = Cast<UDynamicEntryBox>(InWidget))
	{
		if (true == EntryBox->GetAllEntries().IsEmpty())
		{
			ForeachEntryClass(OutTree, EntryBox->GetEntryWidgetClass(), InKey);
		}

		else
		{
			ForeachEntry(OutTree, *EntryBox->GetAllEntries().begin(), InKey);
		}
	}
}

void UGuideMaskRegister::ForeachEntryClass(OUT TMap<int, UWidget*>& OutTree, TSubclassOf<UUserWidget> InEntryClass, int& InKey) const
{
	if (UUserWidget* EntryWidget = CreateWidget<UUserWidget>(GetWorld(), InEntryClass))
	{
		ForeachEntry(OutTree, EntryWidget, InKey);
	}
}

void UGuideMaskRegister::ForeachEntry(OUT TMap<int, UWidget*>& OutTree, UUserWidget* InEntry, int& InKey) const
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
			ConstructWidgetTree(OutTree, Childs[i], InKey);
		}
	}
}

void UGuideMaskRegister::CreatePreviewLayer(const FGeometry& InViewportGeometry)
{
	const UGuideMaskSettings* Settings = GetDefault<UGuideMaskSettings>();
	if (!ensureAlways(Settings))
	{
		return;
	}

	if (UGuideLayerBase* Layer = CreateWidget<UGuideLayerBase>(GetWorld(), Settings->DefaultLayer.Get()))
	{
		UWidget** FoundWidget = TagWidgetList.Find(PreviewWidgetTag);
		if (nullptr != FoundWidget && nullptr != *FoundWidget)
		{
			int Key = 1;
			ConstructWidgetTree(OUT TreeLevels, *FoundWidget, Key);

			UWidget** Widget = TreeLevels.Find(PreviewTreeLevel);

			if (Widget && *Widget)
			{
				SetLayer(Layer);

				UWidget* PreviewWidget = nullptr;

				if (UListViewBase* ListView = Cast<UListViewBase>(*Widget))
				{
					PreviewWidget = false == ListView->GetDisplayedEntryWidgets().IsEmpty() ? *ListView->GetDisplayedEntryWidgets().begin() : nullptr;
				}

				else if (UDynamicEntryBox* EntryBox = Cast<UDynamicEntryBox>(*Widget))
				{
					PreviewWidget = false == EntryBox->GetAllEntries().IsEmpty() ? *EntryBox->GetAllEntries().begin() : nullptr;
				}

				Layer->SetGuide(InViewportGeometry, nullptr != PreviewWidget ? PreviewWidget : *Widget);
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

	if (UWidgetBlueprint* WidgetBlueprint = GetTypedOuter<UWidgetBlueprint>())
	{
		if (WidgetBlueprint->GeneratedClass)
		{
			if (WidgetBlueprint->GeneratedClass->ImplementsInterface(UEntryGuideIdentifiable::StaticClass()))
			{
				CompileLog.Error(LOCTEXT("GuideMaskRegister", "Do not Inherited EntryGuideIdentifiable Interface!"));
			}

			if (WidgetBlueprint->GeneratedClass->ImplementsInterface(UUserObjectListEntry::StaticClass()))
			{
				CompileLog.Error(LOCTEXT("GuideMaskRegister", "Do not Inherited UserObjectListEntry Interface!"));
			}
		}
	}


	for (auto& [Tag, Widget] : TagWidgetList)
	{
		if (UListViewBase* ListView = Cast<UListViewBase>(Widget))
		{
			UClass* WidgetClass = ListView->GetEntryWidgetClass();
			if (WidgetClass && false == WidgetClass->ImplementsInterface(UEntryGuideIdentifiable::StaticClass()))
			{
				CompileLog.Error(FText::Format(LOCTEXT("GuideMaskRegister", 
					"{0} Class doesn't implement EntryGuideIdentifiable Interface!"), 
					FText::FromString(WidgetClass->GetName())));
			}

		}

		else if (UDynamicEntryBox* EntryBox = Cast<UDynamicEntryBox>(Widget))
		{
			UClass* WidgetClass = EntryBox->GetEntryWidgetClass();
			if (WidgetClass && false == WidgetClass->ImplementsInterface(UEntryGuideIdentifiable::StaticClass()))
			{
				CompileLog.Error(FText::Format(LOCTEXT("GuideMaskRegister",
					"{0} Class doesn't implement EntryGuideIdentifiable Interface!"),
					FText::FromString(WidgetClass->GetName())));
			}
		}

		else if (UUserWidget* UserWidget = Cast<UUserWidget>(Widget))
		{
			if (nullptr == UserWidget->GetClass())
			{
				return;
			}

			else if (UWidgetBlueprint* GeneratedWidget = Cast<UWidgetBlueprint>(UserWidget->GetClass()->ClassGeneratedBy))
			{
				if (GeneratedWidget->WidgetTree && GeneratedWidget->WidgetTree->RootWidget)
				{
					if (UGuideMaskRegister* Register = Cast<UGuideMaskRegister>(GeneratedWidget->WidgetTree->RootWidget))
					{
						CompileLog.Error(FText::Format(LOCTEXT("GuideMaskRegister",
							"Do not containing GuideRegister in TagWidgetList. Widget Name : {0}"),
							FText::FromString(UserWidget->GetName())));
					}
				}
			}
		}
	}


}
#endif


bool UGuideMaskRegister::IsContains(const FName& InTag) const
{
	return TagWidgetList.Contains(InTag);
}

UWidget* UGuideMaskRegister::GetTagWidget(const FName& InTag, int Level) const
{
	if (false == IsContains(InTag))
	{
		return nullptr;
	}


	UWidget* Widget = TagWidgetList.FindRef(InTag);
	if (Level != 1)
	{
		int Key = 1;
		TMap<int, UWidget*> GuideWidgetTree;
		ConstructWidgetTree(OUT GuideWidgetTree, Widget, Key);

		if (GuideWidgetTree.Contains(Level))
		{
			return GuideWidgetTree[Level];
		}
	}

	return Widget;
}

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

#if WITH_EDITOR

	TreeLevels.Reset();

	if (TagWidgetList.Contains(PreviewWidgetTag))
	{
		UWidget* Widget = TagWidgetList.FindRef(PreviewWidgetTag);
		int Key = 1;

		ConstructWidgetTree(OUT TreeLevels, Widget, Key);

		if (false == TreeLevels.Contains(PreviewTreeLevel))
		{
			PreviewTreeLevel = 1;
		}
	}

#endif 
}


#undef LOCTEXT_NAMESPACE