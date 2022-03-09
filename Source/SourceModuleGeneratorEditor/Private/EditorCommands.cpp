// Copyroght 2022 U.N.Owen, All Rights Reserved.

#include "EditorCommands.h"
#include "SourceModuleGeneratorEditor.h"
#include "Styling/SlateStyle.h"

FEditorCommands::FEditorCommands() : TCommands<FEditorCommands>(TEXT("ContextName"), NSLOCTEXT("TextNamespace", "TextKey", "TextContent"), NAME_None, FSourceModuleGeneratorEditorModule::StyleInstance->GetStyleSetName())
{

}

void FEditorCommands::RegisterCommands()
{

}


