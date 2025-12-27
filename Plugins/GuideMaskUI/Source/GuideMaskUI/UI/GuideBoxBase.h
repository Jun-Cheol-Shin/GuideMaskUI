// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GuideBoxBase.generated.h"

//class UProgressBar;

DECLARE_DYNAMIC_DELEGATE(FOnActionDynamicEvent);

UENUM(BlueprintType)
enum class EGuideActionType : uint8
{
	DownAndUp,
	Hold,

	Drag,
	Swipe_Up,
	Swipe_Down,
	Swipe_Left,
	Swipe_Right,

	None_Action,
};

USTRUCT(BlueprintType)
struct FGuideBoxActionParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGuideActionType ActionType = EGuideActionType::None_Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EGuideActionType::DownAndUp == ActionType || EGuideActionType::Hold == ActionType", EditConditionHides))
	FKey ActivationKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EGuideActionType::DownAndUp != ActionType && EGuideActionType::KeyEvent != ActionType && EGuideActionType::Hold != ActionType", EditConditionHides))
	float DragThresholdVectorSize = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "EGuideActionType::Hold == ActionType", EditConditionHides))
	float HoldSeconds = 0.f;
};


UCLASS()
class GUIDEMASKUI_API UGuideBoxBase : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FOnPostActionSignature)
	FOnPostActionSignature OnPostAction;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void SetGuideWidget(UWidget* InWidget, const FGuideBoxActionParameters& InActionParam = FGuideBoxActionParameters());

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void SetGuideAction(const FGuideBoxActionParameters& InActionParam);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	EGuideActionType GetActionType() const { return ActionParam.ActionType; }

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	double GetCurrentHoldSeconds() const { return ActionParam.HoldSeconds; }

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	FKey GetCurrentActionKey() const { return ActionParam.ActivationKey; }

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	UWidget* GetActionWidget() const { return ActionWidget.IsValid() ? ActionWidget.Get() : nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void ForcedEndAction();
	
protected:
	virtual void OnInitializeBox() {};
	virtual FReply OnStartedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	virtual FReply OnMoved(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	virtual FReply OnEndedClick(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	virtual void OnEndedAction(const FPointerEvent& InEvent = FPointerEvent());
	virtual FReply OnStartedKeyEvent(const FGeometry& InGeometry, const FKeyEvent& InEvent);
	virtual FReply OnEndedKeyEvent(const FGeometry& InGeometry, const FKeyEvent& InEvent);
	
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
	void Clear();
	
	void OnResizedViewport(FViewport* InViewport, uint32 InWindowMode /*?*/);
	void OnChangedVisibility(ESlateVisibility InVisiblity);

	bool IsCorrectSwipe(const FVector2D& InMoveVec);
	bool IsDragType(EGuideActionType InType) const;

	FPointerEvent CreateMouseLikePointerEventFromTouch(const FPointerEvent& InTouchEvent);

protected:
	TWeakObjectPtr<UWidget> ActionWidget = nullptr;

	FGuideBoxActionParameters ActionParam {};

	//FKey ActivationKey {};
	//EGuideActionType ActionType {};
	//double HoldSeconds = 0.f;
	//float DragThreshold = 100.f;

	FVector2D TouchStartPos = FVector2D();
	float ActionDPIScale = 0.f;
	float CorrectedDragThreshold = 0.f;
	double StartTime = 0.f;

	EButtonClickMethod::Type CachedClickMethod = EButtonClickMethod::DownAndUp;
	EButtonTouchMethod::Type CachedTouchMethod = EButtonTouchMethod::DownAndUp;

protected:
	//UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	//UProgressBar* HoldProgressBar = nullptr;
};
