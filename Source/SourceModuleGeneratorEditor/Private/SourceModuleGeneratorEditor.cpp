// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "SourceModuleGeneratorEditor.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Commands/Commands.h"
#include "EditorCommands.h"
#include "ToolMenus.h"

// Define class static members.
TSharedPtr<FSlateStyleSet> FSourceModuleGeneratorEditorModule::StyleInstance = nullptr;
TSharedPtr<FUICommandList> FSourceModuleGeneratorEditorModule::CommandList = nullptr;

void FSourceModuleGeneratorEditorModule::StartupModule()
{
	// This code will execute after you module is loaded into memory; the exact timing is specified in the .uplugin file per-module.

	if (!FSourceModuleGeneratorEditorModule::StyleInstance.IsValid())
	{
		FSourceModuleGeneratorEditorModule::StyleInstance = MakeShared<FSlateStyleSet>("SourceModuleGeneratorEditorSlateStyleSet");
		FSlateStyleRegistry::RegisterSlateStyle(*FSourceModuleGeneratorEditorModule::StyleInstance);
		FEditorCommands::Register();
		FSourceModuleGeneratorEditorModule::CommandList = MakeShareable(new FUICommandList);
		FSourceModuleGeneratorEditorModule::CommandList->MapAction(FEditorCommands::Get().ButtonAction, FExecuteAction::CreateRaw(this, &FSourceModuleGeneratorEditorModule::AddingModuleDialog));
		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSourceModuleGeneratorEditorModule::RegisterMenu));
	}
}

void FSourceModuleGeneratorEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);
	FSlateStyleRegistry::UnRegisterSlateStyle(*FSourceModuleGeneratorEditorModule::StyleInstance);
	ensure(FSourceModuleGeneratorEditorModule::StyleInstance.IsUnique());
	FSourceModuleGeneratorEditorModule::StyleInstance.Reset();
	FEditorCommands::Unregister();
}

IMPLEMENT_MODULE(FSourceModuleGeneratorEditorModule, SourceModuleGeneratorEditor)

void FSourceModuleGeneratorEditorModule::RegisterMenu()
{
	{
		UToolMenu* ToolMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.File");
		{
			FToolMenuSection& Section = ToolMenu->FindOrAddSection("FileProject");
			Section.AddMenuEntryWithCommandList(FEditorCommands::Get().ButtonAction, this->CommandList);
		}
	}
}

void FSourceModuleGeneratorEditorModule::AddingModuleDialog()
{

}

