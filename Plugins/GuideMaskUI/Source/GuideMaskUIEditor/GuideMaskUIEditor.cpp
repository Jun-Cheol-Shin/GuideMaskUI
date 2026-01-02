// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "GuideHierarchyNodeCustomization.h"
#include "GuideMaskRegDetailCustomization.h"
#include "GuideMaskUI/UI/GuideMaskRegister.h"

class FGuideMaskUIEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};


#define LOCTEXT_NAMESPACE "FGuideMaskUIEditorModule"

void FGuideMaskUIEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout(FGuideHierarchyNode::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGuideHierarchyNodeCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UGuideMaskRegister::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FGuideMaskRegDetailCustomization::MakeInstance));
}

void FGuideMaskUIEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout(FGuideHierarchyNode::StaticStruct()->GetFName());
	PropertyModule.UnregisterCustomClassLayout(UGuideMaskRegister::StaticClass()->GetFName());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGuideMaskUIEditorModule, GuideMaskUIEditor)