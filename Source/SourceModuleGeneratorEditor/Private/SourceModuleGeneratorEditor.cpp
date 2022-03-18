// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "SourceModuleGeneratorEditor.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Commands/Commands.h"
#include "EditorCommands.h"
#include "ToolMenus.h"
#include "Widgets/SWindow.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SEditableTextBox.h"

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
	TSharedPtr<SWindow> MainWindow;
	FSlateBrush Brush;
	SAssignNew(MainWindow, SWindow)
	.Title(NSLOCTEXT("SourceModuleGeneratorEditor", "WindowTitle", "Source Module Generator"))
	.SizingRule(ESizingRule::Autosized)
	.AutoCenter(EAutoCenter::PreferredWorkArea)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.VAlign(EVerticalAlignment::VAlign_Top)
		.AutoHeight()
		.Padding(FMargin(2.f))
		[
			SNew(SBorder)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(FMargin(2.f))
				.AutoWidth()
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "WindowContent", "Module Name:"))
				]
				+ SHorizontalBox::Slot()
				.Padding(FMargin(2.f))
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(SEditableTextBox)
				]
			]
		]
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "WindowContent", "SecondLine"))
		]
	]
	;

	if (MainWindow.IsValid())
	{
		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		FSlateApplication::Get().AddModalWindow(MainWindow.ToSharedRef(), MainFrameModule.GetParentWindow());
	}
}

