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

	// Path points to module root folder.
	FString ModuleRootFolderPath;

	// Path points to public folder.
	FString ModulePublicFolderPath;

	// Path points to private folder.
	FString ModulePrivateFolderPath;

	// Project or plugin descriptor file path.
	FString DescriptorFilePath;

	// Environment that can load this module.
	FString HostType;

	// Phase at which this module should be loaded during startup.
	FString LoadingPhase;

	EModuleImplementType ModuleImplementType = EModuleImplementType::NormalModule;

};

