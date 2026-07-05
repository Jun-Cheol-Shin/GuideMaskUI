// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SBox;
class SImage;
class UGuideVisualWidgetHost;
struct FSlateBrush;

class GUIDEMASKUI_API SGuideVisualWidgetHost : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SGuideVisualWidgetHost)
	{
	}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UGuideVisualWidgetHost* InOwner);

	void SetContent(const TSharedRef<SWidget>& InContent);
	void ClearContent();
	void SetImage(const FSlateBrush* InBrush, FLinearColor InColorAndOpacity);

	// InputProcessor로 이동
	//virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override; 
	//virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	//virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	//virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

	//virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;
	//virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;

private:
	TWeakObjectPtr<UGuideVisualWidgetHost> Owner = nullptr;
	TSharedPtr<SBox> ContentBox;
	TSharedPtr<SImage> ImageWidget;
};
