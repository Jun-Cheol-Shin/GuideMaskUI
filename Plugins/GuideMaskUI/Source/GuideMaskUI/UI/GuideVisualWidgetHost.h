// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ContentWidget.h"
#include "Styling/SlateBrush.h"
#include "GuideVisualWidgetHost.generated.h"

class SGuideVisualWidgetHost;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideVisualMouseButtonDown,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InMouseEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideVisualMouseMove,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InMouseEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideVisualMouseButtonUp,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InMouseEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideVisualTouchStarted,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InTouchEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideVisualTouchMoved,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InTouchEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideVisualTouchEnded,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InTouchEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideVisualMouseEnter,
	const FGeometry&, InGeometry,
	const FPointerEvent&, InMouseEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGuideVisualMouseLeave,
	const FPointerEvent&, InMouseEvent);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideVisualKeyDown,
	const FGeometry&, InGeometry,
	const FKeyEvent&, InKeyEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuideVisualKeyUp,
	const FGeometry&, InGeometry,
	const FKeyEvent&, InKeyEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGuideVisualAction, 
	FName, GuideTag);

UENUM(BlueprintType)
enum class EGuideActionType : uint8
{
	DownAndUp = 0,
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBoxAction")
	EGuideActionType ActionType = EGuideActionType::None_Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBoxAction", meta = (EditCondition = "EGuideActionType::DownAndUp == ActionType || EGuideActionType::Hold == ActionType", EditConditionHides))
	FKey ActivationKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBoxAction", meta = (EditCondition = "EGuideActionType::DownAndUp != ActionType && EGuideActionType::None_Action != ActionType && EGuideActionType::Hold != ActionType", EditConditionHides))
	float DragThresholdVectorSize = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuideBoxAction", meta = (EditCondition = "EGuideActionType::Hold == ActionType", EditConditionHides))
	float HoldSeconds = 0.f;
};


UCLASS(meta = (DisplayName = "Guide Visual Widget Host"))
class GUIDEMASKUI_API UGuideVisualWidgetHost : public UContentWidget
{
	GENERATED_BODY()

	friend class SGuideVisualWidgetHost;
	friend class FGuideVisualInputProcessor;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void SynchronizeProperties() override;
	virtual void OnSlotAdded(UPanelSlot* InSlot) override;
	virtual void OnSlotRemoved(UPanelSlot* InSlot) override;

	virtual void OnAction();

public:
	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost|Guide", meta = (WorldContext = "WorldContextObject"))
	void SetGuide(const UObject* WorldContextObject, FName GuideTag, const FGuideBoxActionParameters& InActionParam);

	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost|Guide")
	EGuideActionType GetActionType() const { return ActionParam.ActionType; }

	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost|Guide")
	float GetCurrentHoldSeconds() const { return ActionParam.HoldSeconds; }

	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost|Guide")
	FKey GetCurrentActionKey() const { return ActionParam.ActivationKey; }

	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost|Guide")
	FName GetGuideTag() const { return Tag; }

	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost")
	UWidget* GetVisualWidget() const { return GetContent(); }

	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost|Image")
	void SetImageBrush(const FSlateBrush& InBrush);

	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost|Image")
	FSlateBrush GetImageBrush() const { return ImageBrush; }

	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost|Image")
	void SetImageColorAndOpacity(FLinearColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "GuideVisualWidgetHost|Image")
	FLinearColor GetImageColorAndOpacity() const { return ImageColorAndOpacity; }


private:
	// 이벤트만 Broadcast 시킨 후, Unhandled로 리턴 (버블링이 되도록..)
	bool HandleMouseButtonDown(const FPointerEvent& InMouseEvent);
	bool HandleMouseButtonUp(const FPointerEvent& InMouseEvent);
	bool HandleMouseMove(const FPointerEvent& InMouseEvent, const FVector2D& MovedVector);

	bool HandleTouchStarted(const FPointerEvent& InTouchEvent);
	bool HandleTouchMoved(const FPointerEvent& InTouchEvent, const FVector2D& MovedVector);
	bool HandleTouchEnded(const FPointerEvent& InTouchEvent);

	bool HandleKeyDown(const FKeyEvent& InKeyEvent);
	bool HandleKeyUp(const FKeyEvent& InKeyEvent);

	void HandleMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	void HandleMouseLeave(const FPointerEvent& InMouseEvent);

	void HandleViewportResized(FViewport* InViewport, uint32 InMessage);
	 
private:
	bool IsCorrectSwipe(const FVector2D& InMoveVector) const;
	bool IsDragType(EGuideActionType InType) const;
	void Clear();

public:
	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualMouseButtonDown OnMouseButtonDownEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualMouseMove OnMouseMoveEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualMouseButtonUp OnMouseButtonUpEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualTouchStarted OnTouchStartedEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualTouchMoved OnTouchMovedEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualTouchEnded OnTouchEndedEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualMouseEnter OnMouseEnterEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualMouseLeave OnMouseLeaveEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualKeyDown OnKeyDownEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualKeyUp OnKeyUpEvent;

	UPROPERTY(BlueprintAssignable, Category = "GuideVisualWidgetHost|Events")
	FOnGuideVisualAction OnActionEvent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuideVisualWidgetHost|Image")
	FSlateBrush ImageBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuideVisualWidgetHost|Image", meta = (sRGB = "true"))
	FLinearColor ImageColorAndOpacity = FLinearColor::White;

private:
	TSharedPtr<SGuideVisualWidgetHost> MyVisualHost;

private:
	TSharedPtr<class FGuideVisualInputProcessor> InputProcessor;

	FGuideBoxActionParameters ActionParam;
	FName Tag;

private:
	//double StartTime = 0.f;

	//FVector2D TouchStartPos = FVector2D();
	float ActionDPIScale = 0.f;
	float CorrectedDragThreshold = 0.f;
};
