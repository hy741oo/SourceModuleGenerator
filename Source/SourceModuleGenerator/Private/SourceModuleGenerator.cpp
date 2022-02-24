// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "SourceModuleGenerator.h"

#define LOCTEXT_NAMESPACE "FSourceModuleGeneratorModule"

void FSourceModuleGeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSourceModuleGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSourceModuleGeneratorModule, SourceModuleGenerator)

DEFINE_LOG_CATEGORY(LogSourceModuleGenerator)