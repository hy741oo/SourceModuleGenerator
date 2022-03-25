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
#include "ModuleDeclarer.h"
#include "Widgets/SWindow.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SSpacer.h"
#include "Interfaces/IPluginManager.h"

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
	TArray<TSharedPtr<EHostType::Type>> HostTypeOptionsSource;
	for (EHostType::Type Item = EHostType::Runtime; Item < EHostType::Max; Item = EHostType::Type(Item + 1))
	{
		HostTypeOptionsSource.Add(MakeShared<EHostType::Type>(EHostType::Type(Item)));
	}

	TSharedPtr<STextBlock> CurrentLoadingPhase;
	TArray<TSharedPtr<ELoadingPhase::Type>> LoadingPhaseOptionsSource;
	for (ELoadingPhase::Type Item = ELoadingPhase::EarliestPossible; Item < ELoadingPhase::Max; Item = ELoadingPhase::Type(Item + 1))
	{
		LoadingPhaseOptionsSource.Add(MakeShared<ELoadingPhase::Type>(ELoadingPhase::Type(Item)));
	}

	TSharedPtr<STextBlock> CurrentModuleImplementType;
	TArray<TSharedPtr<EModuleImplementType>> ModuleImplementTypeOptionsSource;
	for (EModuleImplementType Item = EModuleImplementType::NormalModule; Item < EModuleImplementType::Max; Item = EModuleImplementType(int(Item) + 1))
	{
		ModuleImplementTypeOptionsSource.Add(MakeShared<EModuleImplementType>(Item));
	}

	TSharedPtr<SComboBox<TSharedPtr<IPlugin>>> SelectedPlugin;
	TSharedPtr<STextBlock> SelectedPluginText;
	TSharedPtr<STextBlock> PluginHintText;
	TArray<TSharedPtr<IPlugin>> PluginsOptionsSource;
	for (TSharedRef<IPlugin> Item : IPluginManager::Get().GetDiscoveredPlugins())
	{
		if (Item->GetType() == EPluginType::Project)
		{
			PluginsOptionsSource.Add(Item);
		}
	}
	PluginsOptionsSource.Sort
	(
		[](const TSharedPtr<IPlugin>& A, const TSharedPtr<IPlugin>& B)
		{
			return A->GetFriendlyName() < B->GetFriendlyName();
		}
	)
	;

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
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(SComboBox<TSharedPtr<EHostType::Type>>)
			.OptionsSource(&HostTypeOptionsSource)
			.InitiallySelectedItem(*HostTypeOptionsSource.begin())
			.OnGenerateWidget_Lambda
			(
				[](const TSharedPtr<EHostType::Type>& HostType) -> const TSharedRef<SWidget>
				{
					return SNew(STextBlock)
							.Text(FText::FromString(EHostType::ToString(*HostType)))
							;
				}
			)
			.OnSelectionChanged_Lambda
			(
				[&CurrentHostType](const TSharedPtr<EHostType::Type>& HostType, const ESelectInfo::Type& SelectInfo) -> void
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
		+ SGridPanel::Slot(1, 4)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "LoadingPhase", "Loading Phase:"))
		]
		+ SGridPanel::Slot(2, 4)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(SComboBox<TSharedPtr<ELoadingPhase::Type>>)
			.OptionsSource(&LoadingPhaseOptionsSource)
			.InitiallySelectedItem(LoadingPhaseOptionsSource[6])
			.OnGenerateWidget_Lambda
			(
				[](const TSharedPtr<ELoadingPhase::Type>& LoadingPhase) -> const TSharedRef<SWidget>
				{
					return SNew(STextBlock)
							.Text(FText::FromString(ELoadingPhase::ToString(*LoadingPhase)))
							;
				}
			)
			.OnSelectionChanged_Lambda
			(
				[&CurrentLoadingPhase](const TSharedPtr<ELoadingPhase::Type>& LoadingPhase, const ESelectInfo::Type& SelectInfo) -> void
				{
					if (CurrentLoadingPhase.IsValid())
					{
						CurrentLoadingPhase->SetText(FText::FromString(ELoadingPhase::ToString(*LoadingPhase)));
					}
				}
			)
			[
				SAssignNew(CurrentLoadingPhase, STextBlock)
				.Text(FText(FText::FromString(ELoadingPhase::ToString(ELoadingPhase::Default))))
			]
		]
		+ SGridPanel::Slot(1, 5)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("SourceModuleGenerator", "ModuleImplementType", "Module Implement Type:"))
		]
		+ SGridPanel::Slot(2, 5)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(SComboBox<TSharedPtr<EModuleImplementType>>)
			.OptionsSource(&ModuleImplementTypeOptionsSource)
			.InitiallySelectedItem(*ModuleImplementTypeOptionsSource.begin())
			.OnGenerateWidget_Lambda
			(
				[](const TSharedPtr<EModuleImplementType>& Type) -> const TSharedRef<SWidget>
				{
					switch (*Type)
					{
					case EModuleImplementType::NormalModule:
						return SNew(STextBlock).Text(FText::FromString("NormalModule"));
					case EModuleImplementType::GameModule:
						return SNew(STextBlock).Text(FText::FromString("GameModule"));
					default:
						return SNew(STextBlock).Text(FText::FromString("<Invalid module implement type>"));
					}
				}
			)
			.OnSelectionChanged_Lambda
			(
				[&CurrentModuleImplementType](const TSharedPtr<EModuleImplementType>& ModuleImplementType, const ESelectInfo::Type& SelectInfo) -> void
				{
					if (CurrentModuleImplementType.IsValid())
					{
						switch (*ModuleImplementType)
						{
						case EModuleImplementType::NormalModule:
							CurrentModuleImplementType->SetText(FText::FromString("NormalModule"));
							break;
						case EModuleImplementType::GameModule:
							CurrentModuleImplementType->SetText(FText::FromString("GameModule"));
							break;
						default:
							CurrentModuleImplementType->SetText(FText::FromString("<Invalid module implement type>"));
							break;
						}
					}
				}
			)
			[
				SAssignNew(CurrentModuleImplementType, STextBlock)
				.Text(FText::FromString("NormalModule"))
			]
		]
		+ SGridPanel::Slot(1, 6)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "IsItPluginModule", "Is it plugin module:"))
		]
		+ SGridPanel::Slot(2, 6)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(SCheckBox)
			.OnCheckStateChanged_Lambda
			(
				[&PluginHintText, &SelectedPlugin](const ECheckBoxState& State) -> void
				{
					if (State == ECheckBoxState::Checked)
					{
						PluginHintText->SetVisibility(EVisibility::Visible);
						SelectedPlugin->SetVisibility(EVisibility::Visible);
					}
					else
					{
						PluginHintText->SetVisibility(EVisibility::Collapsed);
						SelectedPlugin->SetVisibility(EVisibility::Collapsed);
					}
				}
			)
		]
		+ SGridPanel::Slot(1, 7)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SAssignNew(PluginHintText, STextBlock)
			.Visibility(EVisibility::Collapsed)
			.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "WhichPlugin", "Which plugin:"))
		]
		+ SGridPanel::Slot(2, 7)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SAssignNew(SelectedPlugin, SComboBox<TSharedPtr<IPlugin>>)
			.Visibility(EVisibility::Collapsed)
			.OptionsSource(&PluginsOptionsSource)
			.InitiallySelectedItem(*PluginsOptionsSource.begin())
			.OnGenerateWidget_Lambda
			(
				[](const TSharedPtr<IPlugin>& Plugin) -> TSharedRef<SWidget>
				{
					return SNew(STextBlock).Text(FText::FromString(Plugin->GetFriendlyName()));
				}
			)
			.OnSelectionChanged_Lambda
			(
				[&SelectedPluginText](const TSharedPtr<IPlugin>& Plugin, const ESelectInfo::Type& SelectInfo) -> void
				{
					SelectedPluginText->SetText(FText::FromString(Plugin->GetFriendlyName()));
				}
			)
			[
				SAssignNew(SelectedPluginText, STextBlock)
				.Text(FText::FromString((*PluginsOptionsSource.begin())->GetFriendlyName()))
			]
		]
		+ SGridPanel::Slot(1, 19)
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.ColumnSpan(2)
		[
			SNew(SButton)
			.Text(NSLOCTEXT("SourceModuleGenerator", "Generate", "Generate"))
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

