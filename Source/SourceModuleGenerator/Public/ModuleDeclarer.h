// Copyright 2022 U.N.Owen, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EModuleImplementType : uint8
{
	// Regular module implement type.
	NormalModule = 0,

	// Module that including gameplay code.
	GameModule,
};

// Information for creating new module.
struct FModuleDeclarer
{
	FString CopyrightMessage;

	FString ModuleName;

	EModuleImplementType ModuleImplementType = EModuleImplementType::NormalModule;

	// Path points to public folder.
	FString ModulePublicPath;

	// Path points to private folder.
	FString ModulePrivatePath;
};

