// Copyright 2022 U.N.Owen, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Information for creating new module.
struct FModuleDeclarer
{
	FString CopyrightMessage;

	FString ModuleName;
};

enum class EModuleImplementType : uint8
{
	// Regular module implement type.
	NormalModule = 0,

	// Module that including gameplay code.
	GameModule,
};
