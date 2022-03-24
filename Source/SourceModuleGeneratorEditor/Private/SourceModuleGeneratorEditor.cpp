// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "SourceModuleGeneratorEditor.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Commands/Commands.h"
#include "EditorCommands.h"
#include "ToolMenus.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "ModuleDescriptor.h"
#include "Widgets/SWindow.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SSpacer.h"

DEFINE_LOG_CATEGORY(LogSourceModuleGeneratorEditor)

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
	TSharedPtr<SEditableTextBox> CopyrightMessage;
	TSharedPtr<SEditableTextBox> ModuleName;

	TSharedPtr<STextBlock> CurrentHostType;
	TArray<TSharedPtr<EHostType::Type>> OptionsSource;
	for (EHostType::Type Item = EHostType::Runtime; Item < EHostType::Max; Item = EHostType::Type(Item + 1))
	{
		OptionsSource.Add(MakeShared<EHostType::Type>(EHostType::Type(Item)));
	}

	SAssignNew(MainWindow, SWindow)
	.Title(NSLOCTEXT("SourceModuleGeneratorEditor", "WindowTitle", "Source Module Generator"))
	.SizingRule(ESizingRule::Autosized)
	.AutoCenter(EAutoCenter::PreferredWorkArea)
	[
		SNew(SGridPanel)
		.FillColumn(2, 1.0f)
		+ SGridPanel::Slot(1, 1)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "CopyrightMessage", "Copyright Message:"))
		]
		+ SGridPanel::Slot(2, 1)
		.Padding(FMargin(2.0f))
		[
			SAssignNew(CopyrightMessage, SEditableTextBox)
		]
		+ SGridPanel::Slot(1, 2)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "ModuleName", "Module Name:"))
		]
		+ SGridPanel::Slot(2, 2)
		.Padding(FMargin(2.0f))
		[
			SAssignNew(ModuleName, SEditableTextBox)
		]
		+ SGridPanel::Slot(1, 3)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "HostType", "Host Type:"))
		]
		+ SGridPanel::Slot(2, 3)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Left)
		[
			SNew(SComboBox<TSharedPtr<EHostType::Type>>)
			.OptionsSource(&OptionsSource)
			.InitiallySelectedItem(*OptionsSource.begin())
			.OnGenerateWidget_Lambda
			(
				[](const TSharedPtr<EHostType::Type> Type) -> const TSharedRef<SWidget>
				{
					return SNew(STextBlock)
							.Text(FText::FromString(EHostType::ToString(*Type)))
							;
				}
			)
			.OnSelectionChanged_Lambda
			(
				[&CurrentHostType](const TSharedPtr<EHostType::Type> HostType, const ESelectInfo::Type SelectInfo) -> void
				{
					if (CurrentHostType.IsValid())
					{
						CurrentHostType->SetText(FText::FromString(EHostType::ToString(*HostType)));
					}
				}
			)
			[
				SAssignNew(CurrentHostType, STextBlock)
				.Text(FText(FText::FromString(EHostType::ToString(EHostType::Runtime))))
			]
		]
	]
	;
	if (MainWindow.IsValid())
	{
		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		FSlateApplication::Get().AddModalWindow(MainWindow.ToSharedRef(), MainFrameModule.GetParentWindow());
	}
	else
	{
		UE_LOG(LogSourceModuleGeneratorEditor, Error, TEXT("Generate main window failed."));
	}
}

