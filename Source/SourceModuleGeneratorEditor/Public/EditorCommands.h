// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SourceModuleGeneratorEditor.h"

class FEditorCommands : public TCommands<FEditorCommands>
{
public:
	FEditorCommands() : TCommands<FEditorCommands>(TEXT("ContextName"), NSLOCTEXT("TextNamespace", "TextKey", "TextContent"), NAME_None, FSourceModuleGeneratorEditorModule::StyleInstance->GetStyleSetName())
	{
		// Empty body.
	}

	virtual void RegisterCommands() override
	{
		// Empty body.
	}
};
