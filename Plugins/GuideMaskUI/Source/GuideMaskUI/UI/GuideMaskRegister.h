// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ContentWidget.h"

#include "GuideMaskRegister.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(UWidget*, FOnGenerateVisualWidgetDynamic, const UWidget*, SourceWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowGuideDynamic, FName, Tag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionGuideDynamic, FName, Tag);


class SOverlay;

USTRUCT(BlueprintType)
struct FGuideHierarchyNode
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GuideHierarchyNode", meta = (DisplayName = "Entry Based Container"))
	UWidget* Container = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GuideHierarchyNode", meta = (DisplayName = "Children"))
	TArray<UWidget*> Children {};
};

/*
*	- todo list
*	1.	GuideBoxBase에 원본 위젯을 직접 전달하지 않는다. DragDropOperator 마냥 Register가 있는 위젯쪽에서
*		가이드할 위젯을 생성시켜서 전달한다. (새로운 위젯을 전달) -> GuideVisualWidget을 생성하는 느낌으로..
*		단, 원본 위젯은 참조할 수 있도록 하여 크기와 회전등등 비쥬얼 위젯이 원본과 같게 세팅한다.
*
*	2. Register에 이벤트 함수를 생성한다.
*		- OnStartGuide(FName Tag)		-> Guide Layer가 켜져있을 때 (액션이 들어가기 전)
*		- OnAction(FName Tag)			-> Guide Layer가 켜지고 사용자가 액션을 들어갔을 때
*		- OnCompleteGuide(FName Tag)	-> Action이 들어가고 난 후
*	각 이벤트 함수에는 Tag를 매개변수로 전달하고, Tag마다 이벤트를 다르게 호출시킬 수 있도록 사용자에게 역할를 부여한다.
* 
*/


UCLASS(meta = (DisplayName = "Guide Mask Register"))
class GUIDEMASKUI_API UGuideMaskRegister : public UContentWidget
{
	GENERATED_BODY()

	friend class UGuideLayerBase;

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "GuideMaskRegister")
	bool IsContains(const FName& InTag) const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "GuideMaskRegister")
	TArray<FName> GetTagList() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "GuideMaskRegister")
	UWidget* GetTagWidget(const FName& InGuideTag) const;

	bool GetGuideWidgetTree(OUT TArray<FGuideHierarchyNode>& OutWidgetTree, const FName& InGuideTag);
	bool GetGuideWidgetList(OUT TArray<UWidget*>& OutWidgetList, const FName& InGuideTag);

	FOnShowGuideDynamic& OnShowGuide();
	FOnActionGuideDynamic& OnActionGuide();

protected:
	virtual UWidget* OnGenerateVisualWidget(const UWidget* InSourceWidget) const;
	virtual void OnShow(FName InTag);
	virtual void OnAction(FName InTag);

private:
	void SetLayer(UWidget* InLayer);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;	 
	virtual void SynchronizeProperties() override;

protected:
	void ConstructWidgetTree(OUT TArray<FGuideHierarchyNode>& OutNodeTree, UWidget* InWidget) const;

#if WITH_EDITOR
	UFUNCTION(BlueprintCosmetic, CallInEditor, meta = (Category = "Guide Mask Preview Option", DisplayName = "Show Preview"))
	void ShowPreviewDebug();

	UFUNCTION(BlueprintCosmetic, CallInEditor, meta = (Category = "Guide Mask Preview Option", DisplayName = "Hide Preview"))
	void HidePreviewDebug();

	virtual const FText GetPaletteCategory() override;
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION()
	TArray<FName> GetTagOptions() const;

	UFUNCTION()
	TArray<FName> GetNestedWidgetOptions() const;

	void CreatePreviewLayer(const FGeometry& InViewportGeometry);
#endif

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "GuideMaskRegister", meta = (GetOptions = "GetTagOptions", DisplayName = "Tag"))
	FName PreviewTag;

	UPROPERTY(EditInstanceOnly, Category = "GuideMaskRegister", meta = (GetOptions = "GetNestedWidgetOptions", DisplayName = "Target Widget"))
	FName PreviewWidget;

	UPROPERTY(VisibleInstanceOnly, Category = "GuideMaskRegister")
	TArray<FGuideHierarchyNode> WidgetHierarchy {};

#endif
	UPROPERTY(EditInstanceOnly, Category = "GuideMaskRegister")
	TMap<FName, UWidget*> TagWidgetList;

private:
	TSharedPtr<SOverlay> Overlay;
	
	UPROPERTY(Transient)
	UWidget* LayerContent = nullptr;


private:
	UPROPERTY(BlueprintReadWrite, Category = Events, meta = (AllowPrivateAccess, DisplayName = "On Generate Visual Widget"))
	FOnGenerateVisualWidgetDynamic BP_OnGenerateVisualWidget;

	UPROPERTY(BlueprintAssignable, Category = Events, meta = (AllowPrivateAccess, DisplayName = "On Show Guide"))
	FOnShowGuideDynamic BP_OnShowGuide;

	UPROPERTY(BlueprintAssignable, Category = Events, meta = (AllowPrivateAccess, DisplayName = "On Action Guide"))
	FOnActionGuideDynamic BP_OnActionGuide;

};
