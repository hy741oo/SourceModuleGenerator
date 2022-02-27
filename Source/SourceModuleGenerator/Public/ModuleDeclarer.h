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

enum class EModuleDescriptorType : uint8
{
	ProjectDescriptor = 0,

	PluginDescriptor
};

// Information for creating new module.
struct FModuleDeclarer
{
	FString CopyrightMessage;

	FString ModuleName;

	EModuleImplementType ModuleImplementType = EModuleImplementType::NormalModule;

	EModuleDescriptorType ModuleDescriptorType = EModuleDescriptorType::ProjectDescriptor;

	// Path points to module root folder.
	FString ModuleRootFolderPath;

	// Path points to public folder.
	FString ModulePublicFolderPath;

	// Path points to private folder.
	FString ModulePrivateFolderPath;

	// Project or plugin descriptor file path.
	FString DescriptorFilePath;
};

