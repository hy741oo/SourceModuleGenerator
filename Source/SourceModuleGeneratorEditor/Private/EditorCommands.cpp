// Copyroght 2022 U.N.Owen, All Rights Reserved.

#include "EditorCommands.h"
#include "SourceModuleGeneratorEditor.h"
#include "Styling/SlateStyle.h"


FEditorCommands::FEditorCommands() : TCommands<FEditorCommands>(TEXT("ContextName"), NSLOCTEXT("TextNameSpace", "TextKey", "TextContent"), NAME_None, FSourceModuleGeneratorEditorModule::StyleInstance->GetStyleSetName())
{
	// Empty.
}

#define LOCTEXT_NAMESPACE "TextNameSpace"

void FEditorCommands::RegisterCommands()
{
	UI_COMMAND(this->ButtonAction, "FriendlyName", "Description", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE

