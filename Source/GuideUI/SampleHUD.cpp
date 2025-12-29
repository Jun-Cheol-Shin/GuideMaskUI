// Fill out your copyright notice in the Description page of Project Settings.


#include "SampleHUD.h"
#include "SampleListEntry.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/ComboBoxString.h"
#include "Components/SpinBox.h"

#include "Kismet/KismetInputLibrary.h"
#include "Kismet/KismetStringLibrary.h"

#include "GuideMaskUI/UI/GuideMaskRegister.h"
#include "GuideMaskUI/GuideMaskUIFunctionLibrary.h"


int USampleHUD::GetWidgetTreeLevel() const
{
	if (nullptr != LevelComboBox)
	{
		return FMath::Max(1, LevelComboBox->GetSelectedIndex() + 1);
	}

	return 1;
}

FName USampleHUD::GetSelectedTag() const
{
	if (nullptr != TagComboBox)
	{
		return FName(TagComboBox->GetSelectedOption());
	}

	return FName();
}

UObject* USampleHUD::GetSampleListItem(UWidget* InTagWidget) const
{
	if (UListView* ListView = Cast<UListView>(InTagWidget))
	{
		UObject* const* FoundItem = SampleListView->GetListItems().FindByPredicate([](UObject* InItem)
			{
				if (USampleListItem* ListItem = Cast<USampleListItem>(InItem))
				{
					return ListItem->ItemId == 10;
				}

				return false;
			});

		return nullptr != FoundItem && nullptr != *FoundItem ? *FoundItem : nullptr;
	}

	return nullptr;
}

void USampleHUD::NativeConstruct()
{
	Super::NativeConstruct();

	/*
	if (nullptr != GuideButton)
	{
		GuideButton->OnClicked.AddDynamic(this, &USampleHUD::OnClickedGuide);
	}
	*/

	if (nullptr != LevelComboBox)
	{
		LevelComboBox->OnSelectionChanged.AddDynamic(this, &USampleHUD::OnLevelSelectionChanged);
	}

	if (nullptr != ActionKeyComboBox)
	{
		ActionKeyComboBox->OnSelectionChanged.AddDynamic(this, &USampleHUD::OnActionKeySelectionChanged);

		if (DisplayToKey.Contains(ActionKeyComboBox->GetSelectedOption()))
		{
			SelectedKey = DisplayToKey[ActionKeyComboBox->GetSelectedOption()];
		}
	}

	if (nullptr != DragValueBox)
	{
		DragValueBox->OnValueChanged.AddDynamic(this, &USampleHUD::OnChangedDragValue);
		DragValue = DragValueBox->GetValue();
	}

	if (nullptr != HoldTimeBox)
	{
		HoldTimeBox->OnValueChanged.AddDynamic(this, &USampleHUD::OnChangedHoldTime);
		HoldTime = HoldTimeBox->GetValue();
	}

	Initialize_SampleListView();
	Initialize_TagComboBox();
	Initialize_ActionTypeComboBox();
}

void USampleHUD::NativeDestruct()
{
	Super::NativeDestruct();

	if (nullptr != SampleListView)
	{
		SampleListView->ClearListItems();
	}
}

void USampleHUD::OnTagSelectionChanged(FString InSelectedItem, ESelectInfo::Type InType)
{
	if (nullptr != LevelComboBox)
	{
		LevelComboBox->ClearOptions();
		TMap<int, UWidget*> NewTree;

		UWidget* TaggedWidget = UGuideMaskUIFunctionLibrary::GetTagWidget(GetWorld(), FName(InSelectedItem));
		if (nullptr != TaggedWidget)
		{
			NewTree = UGuideMaskUIFunctionLibrary::GetWidgetTree(GetWorld(), TaggedWidget);
		}

		TArray<int> KeyList;
		NewTree.GenerateKeyArray(KeyList);

		TArray<UWidget*> WidgetNameList;
		NewTree.GenerateValueArray(WidgetNameList);

		for (int i = 0; i < KeyList.Num(); ++i)
		{
			LevelComboBox->AddOption(FString::Printf(TEXT("%d. %s"), KeyList[i], *WidgetNameList[i]->GetName()));
		}

		LevelComboBox->SetSelectedIndex(0);
	}
}

void USampleHUD::OnActionTypeSelectionChanged(FString InSelectedItem, ESelectInfo::Type InType)
{
	if (nullptr != ActionKeyComboBox)
	{
		DisplayToKey.Reset();
		ActionKeyComboBox->ClearOptions();

		uint8 Value = StaticEnum<EGuideActionType>()->GetValueByName(FName(InSelectedItem));
		EGuideActionType Type = static_cast<EGuideActionType>(Value);

		switch (Type)
		{
		case EGuideActionType::DownAndUp:
		case EGuideActionType::Hold:
		{
			TArray<FKey> AllKeys;
			EKeys::GetAllKeys(AllKeys);

			for (FKey Key : AllKeys)
			{
				if (!UKismetInputLibrary::Key_IsValid(Key))
				{
					continue;
				}

				if (Key == EKeys::AnyKey)
				{
					continue;
				}

				if (Key.IsDeprecated() || Key.IsVirtual() || Key.IsTouch())
				{
					continue;
				}

				if (!Key.IsBindableInBlueprints())
				{
					continue;
				}

				if (UKismetInputLibrary::Key_IsGamepadKey(Key) ||
					UKismetInputLibrary::Key_IsAnalog(Key) ||
					UKismetInputLibrary::Key_IsAxis1D(Key) ||
					UKismetInputLibrary::Key_IsAxis2D(Key) ||
					UKismetInputLibrary::Key_IsAxis3D(Key) ||
					UKismetInputLibrary::Key_IsButtonAxis(Key))
				{
					continue;            
				}

				const FText DisplayText = Key.GetDisplayName();
				FString Display = DisplayText.ToString();

				DisplayToKey.Emplace(Display, Key);
				ActionKeyComboBox->AddOption(Display);
			}

		}
		break;

		default:
		case EGuideActionType::None_Action:
		case EGuideActionType::Drag:
		case EGuideActionType::Swipe_Up:
		case EGuideActionType::Swipe_Down:
		case EGuideActionType::Swipe_Left:
		case EGuideActionType::Swipe_Right:
		{
			//
		}
		break;
		}

		ActionType = Type;
		
		ActionKeyComboBox->SetSelectedIndex(0);
	}
}

void USampleHUD::OnActionKeySelectionChanged(FString InSelectedItem, ESelectInfo::Type InType)
{
	if (DisplayToKey.Contains(InSelectedItem))
	{
		SelectedKey = DisplayToKey[InSelectedItem];
	}
}

void USampleHUD::OnChangedDragValue(float InValue)
{
	DragValue = InValue;
}

void USampleHUD::OnChangedHoldTime(float InValue)
{
	HoldTime = InValue;
}

void USampleHUD::OnLevelSelectionChanged(FString InSelectedItem, ESelectInfo::Type InType)
{
	//
}

void USampleHUD::Initialize_ActionTypeComboBox()
{
	if (nullptr != ActionTypeComboBox)
	{
		ActionTypeComboBox->OnSelectionChanged.AddDynamic(this, &USampleHUD::OnActionTypeSelectionChanged);
		for (uint8 i = 0; i < static_cast<uint8>(EGuideActionType::None_Action) - 1; ++i)
		{
			FString String = StaticEnum<EGuideActionType>()->GetNameStringByValue(i);
			ActionTypeComboBox->AddOption(String);
		}

		ActionTypeComboBox->SetSelectedIndex(0);
		ActionType = static_cast<EGuideActionType>(0);
	}
}
void USampleHUD::Initialize_SampleListView()
{
	if (nullptr != SampleListView)
	{
		TArray<UObject*> ListItems;

		for (int i = 1; i <= 50; ++i)
		{
			USampleListItem* ListItem = NewObject<USampleListItem>();
			ListItem->ItemId = i;

			ListItems.Emplace(ListItem);
		}

		SampleListView->SetListItems(ListItems);
	}

}
void USampleHUD::Initialize_TagComboBox()
{
	if (nullptr != TagComboBox)
	{
		TArray<UGuideMaskRegister*> Registers;
		UGuideMaskUIFunctionLibrary::GetAllGuideRegisters(GetWorld(), OUT Registers);

		for (int i = 0; i < Registers.Num(); ++i)
		{
			UGuideMaskRegister* Register = Registers[i];
			if (nullptr == Register)
			{
				continue;
			}

			TArray<FName> TagList = Register->GetTagList();

			for (int j = 0; j < TagList.Num(); ++j)
			{
				FName Tag = TagList[j];
				TagComboBox->AddOption(Tag.ToString());
			}
		}

		TagComboBox->OnSelectionChanged.AddDynamic(this, &USampleHUD::OnTagSelectionChanged);
		TagComboBox->SetSelectedIndex(0);
	}
}


// goto Blueprint graph
/*
void USampleHUD::OnClickedGuide()
{
	// Guide Setting Sample Code

	if (nullptr != WarningText)
	{
		WarningText->SetVisibility(ESlateVisibility::Collapsed);
	}

	FString TagString = TagComboBox->GetSelectedOption();
	FString LevelString = LevelComboBox->GetSelectedOption();

	if (LevelString.IsNumeric())
	{
		int32 Level = UKismetStringLibrary::Conv_StringToInt(LevelString);
		UWidget* Widget = UGuideMaskUIFunctionLibrary::GetTagWidget(GetWorld(), FName(TagString), Level);

		if (UListView* ListView = Cast<UListView>(Widget))
		{
			UObject* const* FoundItem = SampleListView->GetListItems().FindByPredicate([](UObject* InItem)
				{
					if (USampleListItem* ListItem = Cast<USampleListItem>(InItem))
					{
						return ListItem->ItemId == 10;
					}

					return false;
				});

			if (FoundItem && *FoundItem)
			{
				//UGuideMaskUIFunctionLibrary::ShowGuideListEntry(GetWorld(), ListView, *FoundItem, 5.f);
			}

			else
			{
				if (nullptr != WarningText)
				{
					WarningText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}
			}
		}

		else if (nullptr != Widget)
		{
			//UGuideMaskUIFunctionLibrary::ShowGuideWidget(GetWorld(), Widget);
		}

		else
		{
			if (nullptr != WarningText)
			{
				WarningText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	}
	
}
*/
