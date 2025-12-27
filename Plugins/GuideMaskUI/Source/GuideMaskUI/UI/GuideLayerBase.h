// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GuideBoxBase.h"
#include "GuideLayerBase.generated.h"

class USizeBox;
class UCanvasPanel;
class UImage;

class UGuideBoxBase;
struct FGuideBoxActionParameters;




UCLASS()
class GUIDEMASKUI_API UGuideLayerBase : public UUserWidget
{
	GENERATED_BODY()

	friend class UGuideMaskRegister;

protected:
	virtual void OnStartGuide();
	virtual void OnEndGuide();

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void SetGuide(UWidget* InWidget, const FGuideBoxActionParameters& InParam = FGuideBoxActionParameters());

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void SetEnableAnim(bool bIsEnable);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void SetAnimRate(float InRate);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void SetCircularShape(bool bIsEnable);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void SetOpacity(float InOpacity);

protected:
	virtual FReply OnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InEvent);
	virtual FReply OnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InEvent);

	virtual void SetGuide(const FGeometry& InViewportGeometry, UWidget* InWidget);
	virtual void SetGuideLayer(const FVector2D& InScreenSize, const FVector2D& InTargetLoc, const FVector2D& InTargetSize);
	virtual void SetMaterialTransform(const FVector2D& InViewportSize, const FVector2D& InPosiiton, const FVector2D& InWidgetSize);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InEvent) override;
	virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InEvent) override;

	virtual void SynchronizeProperties() override;

private:
	void OnResizedViewport(FViewport* InViewport, uint32 InMessage);
	
protected:
	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "1"))
	float Opacity = 0.8f;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "1"))
	float AnimationRate = 0.8f;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true"))
	bool bUseCircle = false;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true"))
	bool bAnimated = false;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true"))
	FVector2D ScreenPosition;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Preview", AllowPrivateAccess = "true"))
	FVector2D HighlightSize;

protected:	
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCanvasPanel* LayerPanel;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* BlackScreen;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	USizeBox* GuideBoxPanel;

protected:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstance = nullptr;	

	UPROPERTY(Transient)
	TObjectPtr<UGuideBoxBase> BoxBaseWidget = nullptr;

	TWeakObjectPtr<UWidget> GuideWidget = nullptr;
};
