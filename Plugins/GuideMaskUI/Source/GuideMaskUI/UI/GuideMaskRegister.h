// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ContentWidget.h"
#include "GuideMaskRegister.generated.h"

/**
 * 
 */


class SOverlay;


UCLASS(meta = (DisplayName = "Guide Mask Register", Category = "Guide_Mask", AutoExpandCategories = "Guide Mask Setting"))
class GUIDEMASKUI_API UGuideMaskRegister : public UContentWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	bool IsContains(const FName& InTag) const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	UWidget* GetTagWidget(const FName& InTag, int Level = 1) const;

private:
	void SetLayer(UWidget* InLayer);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;	
	virtual void SynchronizeProperties() override;

#if WITH_EDITOR
	void ConstructWidgetTree(OUT TMap<int, UWidget*>& OutTree, UWidget* InWidget, int& InKey) const;
	void ForeachEntryClass(OUT TMap<int, UWidget*>& OutTree, TSubclassOf<UUserWidget> InEntryClass, int& InKey) const;
	void ForeachEntry(OUT TMap<int, UWidget*>& OutTree, UUserWidget* InEntry, int& InKey) const;
	void CreatePreviewLayer(const FGeometry& InViewportGeometry);

	virtual const FText GetPaletteCategory() override;
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;

	UFUNCTION(BlueprintCosmetic, CallInEditor, meta = (Category = "Guide Mask Setting", DisplayName = "Show Preview"))
	void ShowPreviewDebug();

	UFUNCTION(BlueprintCosmetic, CallInEditor, meta = (Category = "Guide Mask Setting", DisplayName = "Hide Preview"))
	void HidePreviewDebug();

	UFUNCTION()
	TArray<FName> GetTagOptions() const;
#endif

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, meta = (Category = "Guide Mask Setting", GetOptions = "GetTagOptions", AllowPrivateAccess = "true"))
	FName PreviewWidgetTag;

	UPROPERTY(EditInstanceOnly, meta = (Category = "Guide Mask Setting", GetOptions = "GetTagOptions", AllowPrivateAccess = "true", ClampMin = "1"))
	int PreviewTreeLevel = 1;

	UPROPERTY(VisibleInstanceOnly, meta = (Category = "Guide Mask Setting", AllowPrivateAccess = "true", DisplayAfter = "TagWidgetList"))
	TMap<int /*Node Level*/, UWidget*> TreeLevels {};
#endif

	UPROPERTY(EditInstanceOnly, meta = (Category = "Guide Mask Setting", AllowPrivateAccess = "true"))
	TMap<FName, UWidget*> TagWidgetList;

private:
	TSharedPtr<SOverlay> Overlay;
	
	UPROPERTY(Transient)
	UWidget* LayerContent = nullptr;

};
