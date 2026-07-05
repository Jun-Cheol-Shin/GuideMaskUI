// Fill out your copyright notice in the Description page of Project Settings.

#include "SGuideVisualWidgetHost.h"

#include "GuideVisualWidgetHost.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/SOverlay.h"

void SGuideVisualWidgetHost::Construct(const FArguments& InArgs, UGuideVisualWidgetHost* InOwner)
{
	bCanSupportFocus = true;
	Owner = InOwner;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SAssignNew(ImageWidget, SImage)
		]
		+ SOverlay::Slot()
		[
			SAssignNew(ContentBox, SBox)
			[
				SNullWidget::NullWidget
			]
		]
	];
}

void SGuideVisualWidgetHost::SetContent(const TSharedRef<SWidget>& InContent)
{
	if (ContentBox.IsValid())
	{
		ContentBox->SetContent(InContent);
	}
}

void SGuideVisualWidgetHost::ClearContent()
{
	if (ContentBox.IsValid())
	{
		ContentBox->SetContent(SNullWidget::NullWidget);
	}
}

void SGuideVisualWidgetHost::SetImage(const FSlateBrush* InBrush, FLinearColor InColorAndOpacity)
{
	if (ImageWidget.IsValid())
	{
		ImageWidget->SetImage(InBrush);
		ImageWidget->SetColorAndOpacity(InColorAndOpacity);
	}
}

void SGuideVisualWidgetHost::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (Owner.IsValid())
	{
		Owner->HandleMouseEnter(MyGeometry, MouseEvent);
	}

	SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
}

void SGuideVisualWidgetHost::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	if (Owner.IsValid())
	{
		Owner->HandleMouseLeave(MouseEvent);
	}

	SCompoundWidget::OnMouseLeave(MouseEvent);
}


//FReply SGuideVisualWidgetHost::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	return Owner.IsValid() ? Owner->HandleMouseButtonDown(MyGeometry, MouseEvent) : FReply::Unhandled();
//}
//
//FReply SGuideVisualWidgetHost::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	return Owner.IsValid() ? Owner->HandleMouseMove(MyGeometry, MouseEvent) : FReply::Unhandled();
//}
//
//FReply SGuideVisualWidgetHost::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	return Owner.IsValid() ? Owner->HandleMouseButtonUp(MyGeometry, MouseEvent) : FReply::Unhandled();
//}
//
//FReply SGuideVisualWidgetHost::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
//{
//	return Owner.IsValid() ? Owner->HandleTouchMoved(MyGeometry, InTouchEvent) : FReply::Unhandled();
//}
//
//FReply SGuideVisualWidgetHost::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
//{
//	return Owner.IsValid() ? Owner->HandleTouchEnded(MyGeometry, InTouchEvent) : FReply::Unhandled();
//}
//FReply SGuideVisualWidgetHost::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	return Owner.IsValid() ? Owner->HandleMouseButtonDown(MyGeometry, MouseEvent) : FReply::Unhandled();
//}
//
//
//FReply SGuideVisualWidgetHost::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
//{
//	return Owner.IsValid() ? Owner->HandleTouchStarted(MyGeometry, InTouchEvent) : FReply::Unhandled();
//}
//FReply SGuideVisualWidgetHost::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
//{
//	return Owner.IsValid() ? Owner->HandleKeyDown(MyGeometry, KeyEvent) : FReply::Unhandled();
//}
//
//FReply SGuideVisualWidgetHost::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
//{
//	return Owner.IsValid() ? Owner->HandleKeyUp(MyGeometry, KeyEvent) : FReply::Unhandled();
//}