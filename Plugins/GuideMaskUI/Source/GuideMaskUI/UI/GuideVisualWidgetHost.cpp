// Fill out your copyright notice in the Description page of Project Settings.

#include "GuideVisualWidgetHost.h"
#include "SGuideVisualWidgetHost.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/PanelSlot.h"

#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "Runtime/Launch/Resources/Version.h"

class FGuideVisualInputProcessor : public IInputProcessor
{
public:
	FGuideVisualInputProcessor(UGuideVisualWidgetHost* InOwner)
		: Owner(InOwner)
	{
	}

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		if (false == Owner.IsValid())
		{
			return false;
		}


		StartPos = Owner->GetCachedGeometry().AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		bStartClick = true == MouseEvent.IsTouchEvent() ? 
			Owner->HandleTouchStarted(MouseEvent, StartPos) : Owner->HandleMouseButtonDown(MouseEvent, StartPos);

		return false;
	}

	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		if (false == Owner.IsValid() || false == bStartClick)
		{
			return false;
		}

		FVector2D CurrentPosition = Owner->GetCachedGeometry().AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		FVector2D MovedPos = CurrentPosition - StartPos;
		true == MouseEvent.IsTouchEvent() ? 
			Owner->HandleTouchMoved(MouseEvent, MovedPos) : Owner->HandleMouseMove(MouseEvent, MovedPos);

		return false;
	}

	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		if (false == Owner.IsValid() || false == bStartClick)
		{
			return false;
		}

		true == MouseEvent.IsTouchEvent() ? Owner->HandleTouchEnded(MouseEvent) : Owner->HandleMouseButtonUp(MouseEvent);
		Clear();

		return false;
	}

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		if (Owner.IsValid())
		{
			bStartClick = Owner->HandleKeyDown(InKeyEvent);
		}

		return false;
	}

	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		if (false == Owner.IsValid() || false == bStartClick)
		{
			return false;
		}

		Owner->HandleKeyUp(InKeyEvent);
		Clear();

		return false;
	}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
		if (false == bHold)
		{
			return;
		}

		HoldDeltaTime += DeltaTime;

		if (HoldTime <= HoldDeltaTime)
		{
			Clear();

			// Action!
			Owner->OnAction();
		}

	}

public:
	void SetHold(double InHoldTime)
	{
		bHold = true;
		HoldDeltaTime = 0.f;
		HoldTime = InHoldTime;
	}

	void Clear()
	{
		bStartClick = false;
		bHold = false;
		HoldTime = 0.f;
		HoldDeltaTime = 0.f;
		StartPos = FVector2D();
	}

private:
	TWeakObjectPtr<UGuideVisualWidgetHost> Owner;

	bool bStartClick = false;
	bool bHold = false;
	float HoldTime = 0.f;
	float HoldDeltaTime = 0.f;
	FVector2D StartPos = FVector2D();
};



void UGuideVisualWidgetHost::SetImageBrush(const FSlateBrush& InBrush)
{
	ImageBrush = InBrush;

	if (MyVisualHost.IsValid())
	{
		MyVisualHost->SetImage(&ImageBrush, ImageColorAndOpacity);
	}
}

void UGuideVisualWidgetHost::SetImageColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ImageColorAndOpacity = InColorAndOpacity;

	if (MyVisualHost.IsValid())
	{
		MyVisualHost->SetImage(&ImageBrush, ImageColorAndOpacity);
	}
}

void UGuideVisualWidgetHost::SetGuide(const UObject* WorldContextObject, FName GuideTag, const FGuideBoxActionParameters& InActionParam)
{
	Clear();

	Tag = GuideTag;
	ActionParam.ActionType = InActionParam.ActionType;
	ActionParam.ActivationKey = InActionParam.ActivationKey;

	if (true == IsDragType(InActionParam.ActionType))
	{
		ActionParam.DragThresholdVectorSize = InActionParam.DragThresholdVectorSize;

		ActionDPIScale = UWidgetLayoutLibrary::GetViewportScale(WorldContextObject);
		CorrectedDragThreshold = ActionParam.DragThresholdVectorSize * ActionDPIScale;
	}

	else if (EGuideActionType::Hold == InActionParam.ActionType)
	{
		ActionParam.HoldSeconds = InActionParam.HoldSeconds;
	}

	if (ensure(InputProcessor))
	{
		InputProcessor->Clear();
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
		FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, 0);
	}
}

void UGuideVisualWidgetHost::OnAction()
{
	OnActionEvent.Broadcast(Tag);
	Clear();
}

bool UGuideVisualWidgetHost::HandleMouseButtonDown(const FPointerEvent& InMouseEvent, const FVector2D& InLocalStartPos)
{
	const FGeometry& Geometry = GetCachedGeometry();
	if (Geometry.GetLocalSize().IsNearlyZero() || false == Geometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
	{
		return false;
	}

	else if (EGuideActionType::None_Action == ActionParam.ActionType)
	{
		return false;
	}


	if (ensure(InputProcessor.IsValid()) && EGuideActionType::Hold == ActionParam.ActionType)
	{
		InputProcessor->SetHold(ActionParam.HoldSeconds);
	}

	OnMouseButtonDownEvent.Broadcast(GetCachedGeometry(), InMouseEvent);

	return true;
}

bool UGuideVisualWidgetHost::HandleMouseMove(const FPointerEvent& InMouseEvent, const FVector2D& InLocalMovedPos)
{
	switch (ActionParam.ActionType)
	{
	case EGuideActionType::Drag:
	{
		OnMouseMoveEvent.Broadcast(GetCachedGeometry(), InMouseEvent);

		if (CorrectedDragThreshold <= InLocalMovedPos.Size())
		{
			UE_LOG(LogTemp, Warning, TEXT("Complete Drag!"));

			OnAction();
			return true;
		}
	}
	break;

	case EGuideActionType::Swipe_Up:
	case EGuideActionType::Swipe_Down:
	case EGuideActionType::Swipe_Left:
	case EGuideActionType::Swipe_Right:
	{
		if (true == IsCorrectSwipe(InLocalMovedPos))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success Swipe!"));

			OnMouseMoveEvent.Broadcast(GetCachedGeometry(), InMouseEvent);

			if (CorrectedDragThreshold <= InLocalMovedPos.Size())
			{
				UE_LOG(LogTemp, Warning, TEXT("Complete Drag!"));

				OnAction();
				return true;
			}
		}
	}
	break;

	default:
	{
		//OnMouseMoveEvent.Broadcast(GetCachedGeometry(), InMouseEvent);
	}
	break;
	}

	return false;
}

bool UGuideVisualWidgetHost::HandleMouseButtonUp(const FPointerEvent& InMouseEvent)
{
	OnMouseButtonUpEvent.Broadcast(GetCachedGeometry(), InMouseEvent);

	switch (ActionParam.ActionType)
	{
	case EGuideActionType::DownAndUp:
	{
		const FGeometry& Geometry = GetCachedGeometry();
		if (Geometry.GetLocalSize().IsNearlyZero() || false == Geometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
		{
			return false;
		}

		else if (InMouseEvent.GetEffectingButton() == ActionParam.ActivationKey)
		{
			OnAction();
			return true;
		}
	}
	break;
	default:
		break;
	}

	return false;
}

bool UGuideVisualWidgetHost::HandleTouchStarted(const FPointerEvent& InTouchEvent, const FVector2D& InLocalStartPos)
{
	const FGeometry& Geometry = GetCachedGeometry();
	if (Geometry.GetLocalSize().IsNearlyZero() || false == Geometry.IsUnderLocation(InTouchEvent.GetScreenSpacePosition()))
	{
		return false;
	}

	if (EGuideActionType::None_Action == ActionParam.ActionType)
	{
		return false;
	}

	if (ensure(InputProcessor) && EGuideActionType::Hold == ActionParam.ActionType)
	{
		InputProcessor->SetHold(ActionParam.HoldSeconds);
	}

	OnTouchStartedEvent.Broadcast(GetCachedGeometry(), InTouchEvent);

	return true;
}

bool UGuideVisualWidgetHost::HandleTouchMoved(const FPointerEvent& InTouchEvent, const FVector2D& InLocalMovedPos)
{
	// todo : MovedEvent를 조건에 맞을 때만 뿌릴 것인가?

	switch (ActionParam.ActionType)
	{
	case EGuideActionType::Drag:
	{
		OnTouchMovedEvent.Broadcast(GetCachedGeometry(), InTouchEvent);

		if (CorrectedDragThreshold <= InLocalMovedPos.Size())
		{
			UE_LOG(LogTemp, Warning, TEXT("Complete Drag!"));

			OnAction();
			return true;
		}
	}
	break;

	case EGuideActionType::Swipe_Up:
	case EGuideActionType::Swipe_Down:
	case EGuideActionType::Swipe_Left:
	case EGuideActionType::Swipe_Right:
	{
		if (true == IsCorrectSwipe(InLocalMovedPos))
		{
			UE_LOG(LogTemp, Warning, TEXT("Success Swipe!"));

			OnTouchMovedEvent.Broadcast(GetCachedGeometry(), InTouchEvent);

			if (CorrectedDragThreshold <= InLocalMovedPos.Size())
			{
				UE_LOG(LogTemp, Warning, TEXT("Complete Drag!"));

				OnAction();
				return true;
			}
		}
	}
	break;

	default:
	{
		//OnTouchMovedEvent.Broadcast(GetCachedGeometry(), InTouchEvent);
	}
	break;
	}

	return false;
}

bool UGuideVisualWidgetHost::HandleTouchEnded(const FPointerEvent& InTouchEvent)
{
	//if (EGuideActionType::None_Action == ActionParam.ActionType)
	//{
	//	return false;
	//}

	OnTouchEndedEvent.Broadcast(GetCachedGeometry(), InTouchEvent);

	switch (ActionParam.ActionType)
	{
	case EGuideActionType::DownAndUp:
	{
		const FGeometry& Geometry = GetCachedGeometry();
		if (Geometry.GetLocalSize().IsNearlyZero() || false == Geometry.IsUnderLocation(InTouchEvent.GetScreenSpacePosition()))
		{
			return false;
		}

		OnAction();
		return true;
	}
	break;
	default:
		break;
	}

	return false;
}

bool UGuideVisualWidgetHost::HandleKeyDown(const FKeyEvent& InKeyEvent)
{
	if (EGuideActionType::None_Action == ActionParam.ActionType ||
		InKeyEvent.GetKey() != ActionParam.ActivationKey)
	{
		return false;
	}

	if (ensure(InputProcessor.IsValid()) && EGuideActionType::Hold == ActionParam.ActionType)
	{
		InputProcessor->SetHold(ActionParam.HoldSeconds);
	}

	OnKeyDownEvent.Broadcast(GetCachedGeometry(), InKeyEvent);

	return true;
}

bool UGuideVisualWidgetHost::HandleKeyUp(const FKeyEvent& InKeyEvent)
{
	if (EGuideActionType::None_Action == ActionParam.ActionType ||
		InKeyEvent.GetKey() != ActionParam.ActivationKey)
	{
		return false;
	}

	OnKeyUpEvent.Broadcast(GetCachedGeometry(), InKeyEvent);

	switch (ActionParam.ActionType)
	{
	case EGuideActionType::DownAndUp:
	{
		if (InKeyEvent.GetKey() == ActionParam.ActivationKey)
		{
			OnAction();
		}
	}
	break;
	default:
		break;
	}

	return true;
}

void UGuideVisualWidgetHost::HandleMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnMouseEnterEvent.Broadcast(InGeometry, InMouseEvent);
}

void UGuideVisualWidgetHost::HandleMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnMouseLeaveEvent.Broadcast(InMouseEvent);
}

void UGuideVisualWidgetHost::HandleViewportResized(FViewport* InViewport, uint32 InMessage)
{
	if (nullptr == InViewport || nullptr == InViewport->GetClient())
	{
		return;
	}

	ActionDPIScale = InViewport->GetClient()->GetDPIScale();
	CorrectedDragThreshold = ActionParam.DragThresholdVectorSize * ActionDPIScale;
}

TSharedRef<SWidget> UGuideVisualWidgetHost::RebuildWidget()
{
	MyVisualHost = SNew(SGuideVisualWidgetHost, this);
	MyVisualHost->SetImage(&ImageBrush, ImageColorAndOpacity);

	if (UPanelSlot* ContentSlot = GetContentSlot())
	{
		if (ContentSlot->Content)
		{
			MyVisualHost->SetContent(ContentSlot->Content->TakeWidget());
		}
	}

	FViewport::ViewportResizedEvent.RemoveAll(this);
	FViewport::ViewportResizedEvent.AddUObject(this, &UGuideVisualWidgetHost::HandleViewportResized);

	InputProcessor = MakeShared<FGuideVisualInputProcessor>(this);

	return MyVisualHost.ToSharedRef();
}

void UGuideVisualWidgetHost::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MyVisualHost.IsValid())
	{
		MyVisualHost->SetImage(&ImageBrush, ImageColorAndOpacity);
	}
}

void UGuideVisualWidgetHost::ReleaseSlateResources(bool bReleaseChildren)
{
	if (InputProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
		InputProcessor.Reset();
	}

	FViewport::ViewportResizedEvent.RemoveAll(this);
	MyVisualHost.Reset();

	Super::ReleaseSlateResources(bReleaseChildren);
}

void UGuideVisualWidgetHost::OnSlotAdded(UPanelSlot* InSlot)
{
	if (MyVisualHost.IsValid() && InSlot && InSlot->Content)
	{
		MyVisualHost->SetContent(InSlot->Content->TakeWidget());
	}
}

void UGuideVisualWidgetHost::OnSlotRemoved(UPanelSlot* InSlot)
{
	if (MyVisualHost.IsValid())
	{
		MyVisualHost->ClearContent();
	}
}

bool UGuideVisualWidgetHost::IsCorrectSwipe(const FVector2D& InMoveVector) const
{
	float XValue = FMath::Abs(InMoveVector.X);
	float YValue = FMath::Abs(InMoveVector.Y);

	switch (ActionParam.ActionType)
	{
	case EGuideActionType::Swipe_Up:
	{
		if (XValue <= YValue && InMoveVector.Y < 0)
		{
			return true;
		}
	}
	break;
	case EGuideActionType::Swipe_Down:
	{
		if (XValue <= YValue && InMoveVector.Y > 0)
		{
			return true;
		}
	}
	break;
	case EGuideActionType::Swipe_Left:
	{
		if (XValue >= YValue && InMoveVector.X < 0)
		{
			return true;
		}
	}
	break;
	case EGuideActionType::Swipe_Right:
	{
		if (XValue >= YValue && InMoveVector.X > 0)
		{
			return true;
		}
	}
	break;
	default:
		break;
	}

	return false;
}

bool UGuideVisualWidgetHost::IsDragType(EGuideActionType InType) const
{
	return  InType == EGuideActionType::Drag ||
		InType == EGuideActionType::Swipe_Up ||
		InType == EGuideActionType::Swipe_Down ||
		InType == EGuideActionType::Swipe_Left ||
		InType == EGuideActionType::Swipe_Right;
}

void UGuideVisualWidgetHost::Clear()
{
	ActionParam.ActionType = EGuideActionType::None_Action;
	ActionParam.DragThresholdVectorSize = 0.f;
	ActionParam.HoldSeconds = 0.f;

	if (InputProcessor.IsValid())
	{
		InputProcessor->Clear();
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	}

	Tag = NAME_None;
}