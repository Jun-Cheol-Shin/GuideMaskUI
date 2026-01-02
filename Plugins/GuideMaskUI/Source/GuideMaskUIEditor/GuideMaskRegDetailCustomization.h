// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

/**
 * 
 */

class IDetailCategoryBuilder;

class GUIDEMASKUIEDITOR_API FGuideMaskRegDetailCustomization : public IDetailCustomization
{

public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	virtual void CustomizeDetails(const TSharedPtr<IDetailLayoutBuilder>& DetailBuilder) override;

private:
	void RegistProperty(IDetailLayoutBuilder& DetailBuilder, IDetailCategoryBuilder& InCategoryBuilder, const FName& InProperty, const UStruct* ClassOutermost, bool bShouldAutoExpand = false);
	void RegistHierarchyProperty(IDetailLayoutBuilder& DetailBuilder, IDetailCategoryBuilder& InCategoryBuilder, const FName& InProperty, const UStruct* ClassOutermost);
	void RegisterPropertyChangedFullRefresh(IDetailLayoutBuilder& DetailBuilder, const FName Property);

	void ForceRefreshDetailPanel();

	static void SortCustomDetailsCategories(const TMap<FName, IDetailCategoryBuilder*>& AllCategoryMap);

private:
	TWeakPtr<IDetailLayoutBuilder> CachedDetailBuilder;
};
