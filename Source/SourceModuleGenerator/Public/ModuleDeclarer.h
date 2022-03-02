// Copyright 2022 U.N.Owen, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EModuleImplementType : uint8
{
	// Regular module implement type.
	NormalModule = 0,

	// Module that including gameplay code.
	GameModule
};

// Information for creating new module.
struct FModuleDeclarer
{
	FString CopyrightMessage;

	FString ModuleName;

	FString ModuleBuildFilePath;

	FString ModuleHeaderFilePath;

	FString ModuleSourceFilePath;

	FString DescriptorFilePath;

	// Environment that can load this module, Runtime, Editor etc.
	FString HostType;

	// Phase at which this module should be loaded during startup.
	FString LoadingPhase;

	EModuleImplementType ModuleImplementType = EModuleImplementType::NormalModule;
};

