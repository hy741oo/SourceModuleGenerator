// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "SourceModuleGeneratorEditor.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Commands/Commands.h"
#include "EditorCommands.h"
#include "ToolMenus.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "ProjectDescriptor.h"
#include "ModuleDescriptor.h"
#include "ModuleDeclarer.h"
#include "moduleFile.h"
#include "Widgets/SWindow.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SCanvas.h"
#include "Widgets/Notifications/SPopUpErrorText.h"
#include "Widgets/Input/SComboButton.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/MessageDialog.h"
#include "HAL/FileManager.h"

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
	TSharedPtr<SPopupErrorText> PopupErrorText;

	TSharedPtr<STextBlock> CurrentHostType;
	TArray<TSharedPtr<EHostType::Type>> HostTypeOptionsSource;
	for (EHostType::Type Item = EHostType::Runtime; Item < EHostType::Max; Item = EHostType::Type(Item + 1))
	{
		HostTypeOptionsSource.Add(MakeShared<EHostType::Type>(EHostType::Type(Item)));
	}
	HostTypeOptionsSource.Sort
	(
		[](const TSharedPtr<EHostType::Type>& A, const TSharedPtr<EHostType::Type>& B)
		{
			return FString(EHostType::ToString(*A)) < FString(EHostType::ToString(*B));
		}
	)
	;
	TSharedPtr<EHostType::Type>* InitialHostType = HostTypeOptionsSource.FindByPredicate
		(
			[](const TSharedPtr<EHostType::Type>& Type) -> bool
			{
				return *Type == EHostType::Runtime;
			}
		)
	;


	TSharedPtr<STextBlock> CurrentLoadingPhase;
	TArray<TSharedPtr<ELoadingPhase::Type>> LoadingPhaseOptionsSource;
	for (ELoadingPhase::Type Item = ELoadingPhase::EarliestPossible; Item < ELoadingPhase::Max; Item = ELoadingPhase::Type(Item + 1))
	{
		LoadingPhaseOptionsSource.Add(MakeShared<ELoadingPhase::Type>(ELoadingPhase::Type(Item)));
	}
	LoadingPhaseOptionsSource.Sort
	(
		[](const TSharedPtr<ELoadingPhase::Type>& A, const TSharedPtr<ELoadingPhase::Type>& B)
		{
			return FString(ELoadingPhase::ToString(*A)) < FString(ELoadingPhase::ToString(*B));
		}
	)
	;
	TSharedPtr<ELoadingPhase::Type>* InitialLoadingPhase = LoadingPhaseOptionsSource.FindByPredicate
		(
			[](const TSharedPtr<ELoadingPhase::Type>& Type) -> bool
			{
				return *Type == ELoadingPhase::Default;
			}
		)
	;

	TSharedPtr<STextBlock> CurrentModuleImplementType;
	TArray<TSharedPtr<EModuleImplementType::Type>> ModuleImplementTypeOptionsSource;
	for (EModuleImplementType::Type Item = EModuleImplementType::Type::NormalModule; Item < EModuleImplementType::Type::Max; Item = EModuleImplementType::Type(int(Item) + 1))
	{
		ModuleImplementTypeOptionsSource.Add(MakeShared<EModuleImplementType::Type>(Item));
	}
	ModuleImplementTypeOptionsSource.Sort
	(
		[](const TSharedPtr<EModuleImplementType::Type>& A, const TSharedPtr<EModuleImplementType::Type>& B)
		{
			return FString(EModuleImplementType::ToString(*A)) < FString(EModuleImplementType::ToString(*B));
		}
	)
	;
	TSharedPtr<EModuleImplementType::Type>* InitialModuleImplementType = ModuleImplementTypeOptionsSource.FindByPredicate
		(
			[](const TSharedPtr<EModuleImplementType::Type>& Type) -> bool
			{
				return *Type == EModuleImplementType::NormalModule;
			}
		)
	;

	TSharedPtr<SComboButton> ProjectModuleTargetType;
	TSharedPtr<STextBlock> ProjectModuleTargetTypeText;
	TArray<TSharedPtr<FString>> ProjectModuleTargetTypeOptionsSource;
	TMap<TSharedPtr<SCheckBox>, FString> ProjectModuleTargetTypeCheckBoxes;

	{
		TArray<FString> TargetTypes;
		IFileManager::Get().FindFiles(TargetTypes, *FPaths::GameSourceDir(), TEXT(".Target.cs"));
		for (const FString& TargetType : TargetTypes)
		{
			ProjectModuleTargetTypeOptionsSource.Add(MakeShared<FString>(TargetType));
		}
	}
	ProjectModuleTargetTypeOptionsSource.Sort
	(
		[](const TSharedPtr<FString>& A, const TSharedPtr<FString>& B) -> bool
		{
			return *A < *B;
		}
	)
	;
	TSharedPtr<SVerticalBox> MainVerticalBox;
	SAssignNew(MainVerticalBox, SVerticalBox);
	for (const TSharedPtr<FString>& Item : ProjectModuleTargetTypeOptionsSource)
	{
		TSharedPtr<SCheckBox> TempCheckBox;
		FString SimplifyTargetTypeName = Item->Left(Item->Find(TEXT(".")));
		MainVerticalBox->AddSlot()
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
				SAssignNew(TempCheckBox, SCheckBox)
				.IsChecked(Item->Contains("Editor") ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				[
					SNew(STextBlock)
					.Text(FText::FromString(SimplifyTargetTypeName))
				]
		]
		;
		ProjectModuleTargetTypeCheckBoxes.Add(TempCheckBox, *Item);
	}


	TSharedPtr<SComboBox<TSharedPtr<IPlugin>>> SelectedPlugin;
	TSharedPtr<STextBlock> SelectedPluginText;
	TSharedPtr<STextBlock> PluginHintText;
	TSharedPtr<SCheckBox> IsItPluginModule;
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
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(0.9f)
		[
			SNew(SUniformGridPanel)
			+ SUniformGridPanel::Slot(0, 0)
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "CopyrightMessage", "Copyright Message: "))
			]
			+ SUniformGridPanel::Slot(1, 0)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SAssignNew(CopyrightMessage, SEditableTextBox)
				.HintText(FText::FromString("(Optional)"))
			]
			+ SUniformGridPanel::Slot(0, 1)
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "ModuleName", "Module Name: "))
			]
			+ SUniformGridPanel::Slot(1, 1)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SAssignNew(ModuleName, SEditableTextBox)
				.ErrorReporting(SAssignNew(PopupErrorText, SPopupErrorText))
				.OnTextChanged_Lambda
				(
					[&ModuleName](const FText& ChangedText) -> void
					{
						if (ChangedText.ToString().Contains(" "))
						{
							ModuleName->SetError(TEXT("Space character is not allowed in module name."));
						}
						else
						{
							if (FModuleManager::Get().ModuleExists(*ChangedText.ToString()))
							{
								ModuleName->SetError(TEXT("Module has already existed."));
							}
							else
							{
								ModuleName->SetError(TEXT(""));
							}
						}
					}
				)
			]
			+ SUniformGridPanel::Slot(0, 2)
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "HostType", "Host Type: "))
			]
			+ SUniformGridPanel::Slot(1, 2)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SComboBox<TSharedPtr<EHostType::Type>>)
				.OptionsSource(&HostTypeOptionsSource)
				.InitiallySelectedItem(InitialHostType ? *InitialHostType : nullptr)
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
					.Text(InitialHostType ? FText::FromString(EHostType::ToString(**InitialHostType)) : FText::FromString("<Invalid host type>"))
				]
			]
			+ SUniformGridPanel::Slot(0, 3)
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "LoadingPhase", "Loading Phase: "))
			]
			+ SUniformGridPanel::Slot(1, 3)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SComboBox<TSharedPtr<ELoadingPhase::Type>>)
				.OptionsSource(&LoadingPhaseOptionsSource)
				.InitiallySelectedItem(InitialLoadingPhase ? *InitialLoadingPhase : nullptr)
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
					.Text(InitialLoadingPhase ? FText::FromString(ELoadingPhase::ToString(**InitialLoadingPhase)) : FText::FromString("<Invalid loading phase>"))
				]
			]
			+ SUniformGridPanel::Slot(0, 4)
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("SourceModuleGenerator", "ModuleImplementType", "Module Implement Type: "))
			]
			+ SUniformGridPanel::Slot(1, 4)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SComboBox<TSharedPtr<EModuleImplementType::Type>>)
				.OptionsSource(&ModuleImplementTypeOptionsSource)
				.InitiallySelectedItem(InitialModuleImplementType ? *InitialModuleImplementType : nullptr)
				.OnGenerateWidget_Lambda
				(
					[](const TSharedPtr<EModuleImplementType::Type>& Type) -> const TSharedRef<SWidget>
					{
						switch (*Type)
						{
						case EModuleImplementType::Type::NormalModule:
							return SNew(STextBlock).Text(FText::FromString("NormalModule"));
						case EModuleImplementType::Type::GameModule:
							return SNew(STextBlock).Text(FText::FromString("GameModule"));
						default:
							return SNew(STextBlock).Text(FText::FromString("<Invalid module implement type>"));
						}
					}
				)
				.OnSelectionChanged_Lambda
				(
					[&CurrentModuleImplementType](const TSharedPtr<EModuleImplementType::Type>& ModuleImplementType, const ESelectInfo::Type& SelectInfo) -> void
					{
						if (CurrentModuleImplementType.IsValid())
						{
							switch (*ModuleImplementType)
							{
							case EModuleImplementType::Type::NormalModule:
								CurrentModuleImplementType->SetText(FText::FromString("NormalModule"));
								break;
							case EModuleImplementType::Type::GameModule:
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
			+ SUniformGridPanel::Slot(0, 5)
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SAssignNew(ProjectModuleTargetTypeText, STextBlock)
				.IsEnabled(ProjectModuleTargetTypeOptionsSource.Num() != 0)
				.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "TargetType", "Target Type: "))
			]
			+ SUniformGridPanel::Slot(1, 5)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SAssignNew(ProjectModuleTargetType, SComboButton)
				.IsEnabled(ProjectModuleTargetTypeOptionsSource.Num() != 0)
				.ContentPadding(FMargin(2.0f))
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "TargetTypeList", "Target Type List"))
				]
				.MenuContent()
				[
					MainVerticalBox.ToSharedRef()
				]
			]
			+ SUniformGridPanel::Slot(0, 6)
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.IsEnabled(PluginsOptionsSource.Num() != 0)
				.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "Isthisapluginmodule", "Is this a plugin module: "))
			]
			+ SUniformGridPanel::Slot(1, 6)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SAssignNew(IsItPluginModule, SCheckBox)
				.IsEnabled(PluginsOptionsSource.Num() != 0)
				.IsChecked(PluginsOptionsSource.Num() == 0 ? ECheckBoxState::Undetermined : ECheckBoxState::Unchecked)
				.OnCheckStateChanged_Lambda
				(
					[&PluginHintText, &SelectedPlugin, &ProjectModuleTargetTypeText, &ProjectModuleTargetType](const ECheckBoxState& State) -> void
					{
						PluginHintText->SetEnabled(State == ECheckBoxState::Checked);
						SelectedPlugin->SetEnabled(State == ECheckBoxState::Checked);
						ProjectModuleTargetTypeText->SetEnabled(!(State == ECheckBoxState::Checked));
						ProjectModuleTargetType->SetEnabled(!(State == ECheckBoxState::Checked));
					}
				)
			]
			+ SUniformGridPanel::Slot(0, 7)
			.HAlign(EHorizontalAlignment::HAlign_Right)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SAssignNew(PluginHintText, STextBlock)
				.IsEnabled(false)
				.Text(NSLOCTEXT("SourceModuleGeneratorEditor", "WhichPlugin", "Which plugin: "))
			]
			+ SUniformGridPanel::Slot(1, 7)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SAssignNew(SelectedPlugin, SComboBox<TSharedPtr<IPlugin>>)
				.IsEnabled(false)
				.OptionsSource(&PluginsOptionsSource)
				.InitiallySelectedItem(PluginsOptionsSource.Num() == 0 ? nullptr : *PluginsOptionsSource.begin())
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
					.Text_Lambda
					(
						[&PluginsOptionsSource]() -> FText
						{
							if (PluginsOptionsSource.Num() == 0)
							{
								return FText::FromString("<No plugin found>");
							}
							else
							{
								return FText::FromString((*PluginsOptionsSource.begin())->GetFriendlyName());
							}
						}
					)
				]
			]
		]
		+ SVerticalBox::Slot()
		.Padding(FMargin(2.0f))
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.AutoHeight()
		[
			SNew(SButton)
			.Text(NSLOCTEXT("SourceModuleGenerator", "Generate", "Generate"))
			.OnClicked_Lambda
			(
				[&CopyrightMessage, &ModuleName, &CurrentHostType, &CurrentLoadingPhase, &CurrentModuleImplementType, &SelectedPlugin, &IsItPluginModule, &MainWindow, &ProjectModuleTargetTypeCheckBoxes]() -> FReply
				{
					// Check whether module name is correct.
					if (ModuleName->HasError())
					{
						UE_LOG(LogSourceModuleGeneratorEditor, Error, TEXT("Module name has error, Abort generating."));
						return FReply::Handled();
					}
					if (ModuleName->GetText().IsEmpty())
					{
						UE_LOG(LogSourceModuleGeneratorEditor, Error, TEXT("Module name is empty, Abort generating."));
						ModuleName->SetError(TEXT("Module name cannot be empty."));
						return FReply::Handled();
					}

					// Build module declarer that needed by generating function.
					FModuleDeclarer ModuleDeclarer;
					if (CopyrightMessage->GetText().IsEmpty())
					{
						ModuleDeclarer.CopyrightMessage = TEXT("Fill out your copyright notice in the Description page of Project Settings.");
					}
					else
					{
						ModuleDeclarer.CopyrightMessage = CopyrightMessage->GetText().ToString();
					}
					ModuleDeclarer.ModuleName = ModuleName->GetText().ToString();
					ModuleDeclarer.HostType = CurrentHostType->GetText().ToString();
					ModuleDeclarer.LoadingPhase = CurrentLoadingPhase->GetText().ToString();
					ModuleDeclarer.ModuleImplementType = EModuleImplementType::FromString(*CurrentModuleImplementType->GetText().ToString());
					if (IsItPluginModule->GetCheckedState() != ECheckBoxState::Checked)
					{
						FProjectDescriptor ProjectDescriptor;
						FText ProjectLoadFailedReason;
						if (!ProjectDescriptor.Load(FPaths::GetProjectFilePath(), ProjectLoadFailedReason))
						{
							UE_LOG(LogSourceModuleGeneratorEditor, Error, TEXT("Load project file failed, reason is: %s, abort generating."), *ProjectLoadFailedReason.ToString());
							return FReply::Handled();
						}
						else
						{
							if (ProjectDescriptor.Modules.Num() == 0)
							{
								UE_LOG(LogSourceModuleGeneratorEditor, Error, TEXT("There is no primary game module in project, please add \"New C++ Class...\" before add new module in project."));
								FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("There is no primary game module in project, please add \"New C++ Class...\" before add new module in project.")));
								return FReply::Handled();
							}
						}
						ModuleDeclarer.DescriptorFilePath = FPaths::GetProjectFilePath();
						ModuleDeclarer.ModuleBuildFilePath = FPaths::Combine(FPaths::GameSourceDir(), ModuleDeclarer.ModuleName, ModuleDeclarer.ModuleName + TEXT(".Build.cs"));
						ModuleDeclarer.ModuleHeaderFilePath = FPaths::Combine(FPaths::GameSourceDir(), ModuleDeclarer.ModuleName, TEXT("Public"), ModuleDeclarer.ModuleName + TEXT(".h"));
						ModuleDeclarer.ModuleSourceFilePath = FPaths::Combine(FPaths::GameSourceDir(), ModuleDeclarer.ModuleName, TEXT("Private"), ModuleDeclarer.ModuleName + TEXT(".cpp"));

						for (const TPair<TSharedPtr<SCheckBox>, FString>& Item : ProjectModuleTargetTypeCheckBoxes)
						{
							if (Item.Key->GetCheckedState() == ECheckBoxState::Checked)
							{
								ModuleDeclarer.ProjectModuleTargetTypeFilePaths.Add(FPaths::Combine(FPaths::GameSourceDir(), Item.Value));
							}
						}
					}
					else
					{
						TSharedPtr<IPlugin> ThePlugin = SelectedPlugin->GetSelectedItem();
						ModuleDeclarer.DescriptorFilePath = ThePlugin->GetDescriptorFileName();
						ModuleDeclarer.ModuleBuildFilePath = FPaths::Combine(ThePlugin->GetBaseDir(), TEXT("Source"), ModuleDeclarer.ModuleName, ModuleDeclarer.ModuleName + TEXT(".Build.cs"));
						ModuleDeclarer.ModuleHeaderFilePath = FPaths::Combine(ThePlugin->GetBaseDir(), TEXT("Source"), ModuleDeclarer.ModuleName, TEXT("Public"), ModuleDeclarer.ModuleName + TEXT(".h"));
						ModuleDeclarer.ModuleSourceFilePath = FPaths::Combine(ThePlugin->GetBaseDir(), TEXT("Source"), ModuleDeclarer.ModuleName, TEXT("Private"), ModuleDeclarer.ModuleName + TEXT(".cpp"));
					}

					if (!CreateModuleFiles(ModuleDeclarer))
					{
						UE_LOG(LogSourceModuleGeneratorEditor, Error, TEXT("Generate module failed, Abort generating."));
						MainWindow->RequestDestroyWindow();
						return FReply::Handled();
					}
					else
					{
						UE_LOG(LogSourceModuleGeneratorEditor, Log, TEXT("Generate module successful."));
					}

					return FReply::Handled();
				}
			)
		]
	]
	;

	PopupErrorText->SetError(TEXT(""));

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

