// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "SourceModuleGeneratorEditor.h"

void FSourceModuleGeneratorEditorModule::StartupModule()
{
	// This code will execute after you module is loaded into memory; the exact timing is specified in the .uplugin file per-module.
}

void FSourceModuleGeneratorEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

IMPLEMENT_MODULE(FSourceModuleGeneratorEditorModule, SourceModuleGeneratorEditor)
