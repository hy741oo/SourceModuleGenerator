// Copyright 2022 U.N.Owen, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace EModuleImplementType
{
	enum Type : uint8
	{
		// Regular module implement type.
		NormalModule = 0,

		// Module that including gameplay code.
		GameModule,

		// Space holder.
		// NOTE:DO NOT USE THIS ENUM DIRECTLY.
		Max
	};

	// Converts enum type to TCHAR type.
	SOURCEMODULEGENERATOR_API const TCHAR* ToString(const EModuleImplementType::Type Value);

	// Converts TCHAR type to enum type.
	SOURCEMODULEGENERATOR_API const EModuleImplementType::Type FromString(const TCHAR* Value);
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

	EModuleImplementType::Type ModuleImplementType = EModuleImplementType::NormalModule;
};

