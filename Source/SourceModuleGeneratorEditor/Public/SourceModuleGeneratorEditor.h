// Copyright 2022 U.N.Owen, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

// Forward declaration.
class FSlateStyleSet;
class FUICommandList;

class FSourceModuleGeneratorEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	static TSharedPtr<FSlateStyleSet> StyleInstance;
	static TSharedPtr<FUICommandList> CommandList;

public:
	void RegisterMenu();
};

