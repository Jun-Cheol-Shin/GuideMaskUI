// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GuideVisualWidgetHost.h"
#include "GuideBoxBase.generated.h"

//class UProgressBar;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideMouseDown,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InMouseEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideMouseMove,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InMouseEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideMouseUp,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InMouseEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideKeyBoardDown,
	const FGeometry&, InGeometry,
	const FKeyEvent&, InKeyEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideKeyBoardUp,
	const FGeometry&, InGeometry,
	const FKeyEvent&, InKeyEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompleteGuideAction, FName, GuideTag);


UCLASS(meta = (Deprecated, DeprecationMessage = "Use UGuideVisualWidgetHost instead."))
class GUIDEMASKUI_API UGuideBoxBase : public UUserWidget
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable, Category = "GuideBoxBase")
	//void SetGuideWidget(UWidget* InWidget);

	UFUNCTION(BlueprintCallable, Category = "GuideBoxBase")
	void SetGuide(FName InTag, const FGuideBoxActionParameters& InActionParam);

	UFUNCTION(BlueprintCallable, Category = "GuideBoxBase")
	EGuideActionType GetActionType() const { return ActionParam.ActionType; }

	UFUNCTION(BlueprintCallable, Category = "GuideBoxBase")
	float GetCurrentHoldSeconds() const { return ActionParam.HoldSeconds; }

	UFUNCTION(BlueprintCallable, Category = "GuideBoxBase")
	FKey GetCurrentActionKey() const { return ActionParam.ActivationKey; }

	/*UFUNCTION(BlueprintCallable, Category = "GuideBoxBase")
	UWidget* GetActionWidget() const { return ActionWidget.IsValid() ? ActionWidget.Get() : nullptr; }*/

	UFUNCTION(BlueprintCallable, Category = "GuideBoxBase")
	FName GetCurrentGuideTag() const { return GuideTag; }

	UFUNCTION(BlueprintCallable, Category = "GuideBoxBase")
	void ForcedEndAction();
	
protected:
	virtual void NativeOnEndAction(const FPointerEvent& InEvent = FPointerEvent());

	virtual FReply NativeOnStartClickAction(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	virtual FReply NativeOnMoveAction(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	virtual FReply NativeOnEndClickAction(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	virtual FReply NativeOnStartKeyAction(const FGeometry& InGeometry, const FKeyEvent& InEvent);
	virtual FReply NativeOnEndKeyAction(const FGeometry& InGeometry, const FKeyEvent& InEvent);

public:
	UPROPERTY(BlueprintAssignable, Category = "GuideBoxBase|Events")
	FOnGuideMouseDown OnMouseDownEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideBoxBase|Events")
	FOnGuideMouseMove OnMouseMovedEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideBoxBase|Events")
	FOnGuideMouseUp OnMouseUpEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideBoxBase|Events")
	FOnGuideKeyBoardDown OnKeyDownEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideBoxBase|Events")
	FOnGuideKeyBoardUp OnKeyUpEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideBoxBase|Events")
	FOnCompleteGuideAction OnCompleteActionEvent;

	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& InGeometry, float InDeltaTime) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent) override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	FPointerEvent CreateMouseLikePointerEventFromTouch(const FPointerEvent& InTouchEvent);

	void Clear();
	
	void OnResizedViewport(FViewport* InViewport, uint32 InWindowMode /*?*/);
	void OnChangedVisibility(ESlateVisibility InVisiblity);

	bool IsCorrectSwipe(const FVector2D& InMoveVec);
	bool IsDragType(EGuideActionType InType) const;

protected:
	//TWeakObjectPtr<UWidget> ActionWidget = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGuideBoxActionParameters ActionParam {};

private:
	FName GuideTag = FName();
	double StartTime = 0.f;
	FVector2D TouchStartPos = FVector2D();
	float ActionDPIScale = 0.f;
	float CorrectedDragThreshold = 0.f;

	//EButtonClickMethod::Type CachedClickMethod = EButtonClickMethod::DownAndUp;
	//EButtonTouchMethod::Type CachedTouchMethod = EButtonTouchMethod::DownAndUp;
};
