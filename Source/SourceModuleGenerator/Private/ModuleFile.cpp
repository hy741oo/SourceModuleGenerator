// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "ModuleFile.h"
#include "ModuleDeclarer.h"
#include "SourceModuleGenerator.h"
#include "ProjectDescriptor.h"
#include "PluginDescriptor.h"
#include "HAL/FileManager.h"
#include "Logging/StructuredLog.h"

// Internal function declares.
void DeleteGeneratedFiles(const TArray<FString>& InGeneratedFilePaths);

bool CreateModuleFiles(const FModuleDeclarer& InModuleDeclarer)
{
	// 判断描述文件是否存在。
	UE_LOGFMT(LogSourceModuleGenerator, Log, "Adding module descriptor to descriptor file... The expected file path is \"{Path}\"", InModuleDeclarer.DescriptorFilePath);
	if (!(IFileManager::Get().FileExists(*(InModuleDeclarer.DescriptorFilePath))))
	{
		UE_LOGFMT(LogSourceModuleGenerator, Error, "Descriptor file does not exist.");
		return false;
	}

	// 定义一个要在描述文件显示的模块信息。
	FModuleDescriptor ModuleDescriptor;
	ModuleDescriptor.Name = FName(InModuleDeclarer.ModuleName);
	ModuleDescriptor.Type = InModuleDeclarer.HostType;
	ModuleDescriptor.LoadingPhase = InModuleDeclarer.LoadingPhase;

	// 为描述文件添加模块信息。
	FProjectDescriptor ProjectDescriptor;
	FPluginDescriptor PluginDescriptor;
	FText FailReason;

	// 判断当前指定的描述文件是项目的还是插件的，读取对应的描述文件内容并添加新模块信息，最后保存到描述文件。
	if (FPaths::GetProjectFilePath() == InModuleDeclarer.DescriptorFilePath)
	{
		if (!ProjectDescriptor.Load(InModuleDeclarer.DescriptorFilePath, FailReason))
		{
			UE_LOGFMT(LogSourceModuleGenerator, Error, "Load project descriptor failed, fail reason:{Reason}.", FailReason.ToString());
			return false;
		}

		ProjectDescriptor.Modules.Add(MoveTemp(ModuleDescriptor));
		if (!ProjectDescriptor.Save(InModuleDeclarer.DescriptorFilePath, FailReason))
		{
			UE_LOGFMT(LogSourceModuleGenerator, Error, "Save project descriptor failed, fail reason:{Reason}.", FailReason.ToString());
			return false;
		}
	}
	else
	{
		if (!PluginDescriptor.Load(InModuleDeclarer.DescriptorFilePath, FailReason))
		{
			UE_LOGFMT(LogSourceModuleGenerator, Error, "Load plugin descriptor failed, fail reason:{Reason}.", FailReason.ToString());
			return false;
		}

		PluginDescriptor.Modules.Add(MoveTemp(ModuleDescriptor));
		if (!PluginDescriptor.Save(InModuleDeclarer.DescriptorFilePath, FailReason))
		{
			UE_LOGFMT(LogSourceModuleGenerator, Error, "Save plugin descriptor failed, fail reason:{Reason}.", FailReason.ToString());
			return false;
		}
	}

	// Storage temporary generated files path.
	TArray<FString> GeneratedFilePaths;

	// Generate module header file.
	TArray<FString> ModuleContents;
	ModuleContents.Add(TEXT("// ") + InModuleDeclarer.CopyrightMessage);
	ModuleContents.Add(FString());
	ModuleContents.Add(TEXT("#pragma once"));
	ModuleContents.Add(FString());
	ModuleContents.Add(TEXT("#include \"CoreMinimal.h\""));
	ModuleContents.Add(TEXT("#include \"Modules/ModuleManager.h\""));
	ModuleContents.Add(FString());
	ModuleContents.Add(FString::Printf(TEXT("class F%sModule : public IModuleInterface"), *(InModuleDeclarer.ModuleName)));
	ModuleContents.Add(TEXT("{"));
	ModuleContents.Add(TEXT("public:"));
	ModuleContents.Add(FString());
	ModuleContents.Add(TEXT("\t/** IModuleInterface implementation */"));
	ModuleContents.Add(TEXT("\tvirtual void StartupModule() override;"));
	ModuleContents.Add(TEXT("\tvirtual void ShutdownModule() override;"));
	ModuleContents.Add(TEXT("};"));

	UE_LOGFMT(LogSourceModuleGenerator, Log, "Creating module header file... The expected file path is \"{Path}\"", InModuleDeclarer.ModuleHeaderFilePath);
	if (!FFileHelper::SaveStringArrayToFile(ModuleContents, *(InModuleDeclarer.ModuleHeaderFilePath)))
	{
		UE_LOGFMT(LogSourceModuleGenerator, Error, "Creating module header file failed.");
		return false;
	}
	GeneratedFilePaths.Add(InModuleDeclarer.ModuleHeaderFilePath);

	// Generate module source file.
	ModuleContents.Empty();
	ModuleContents.Add(TEXT("// ") + InModuleDeclarer.CopyrightMessage);
	ModuleContents.Add(FString());
	ModuleContents.Add(FString::Printf(TEXT("#include \"%s.h\""), *(InModuleDeclarer.ModuleName)));
	ModuleContents.Add(FString());
	ModuleContents.Add(FString::Printf(TEXT("void F%sModule::StartupModule()"), *(InModuleDeclarer.ModuleName)));
	ModuleContents.Add(TEXT("{"));
	ModuleContents.Add(TEXT("\t// This code will execute after you module is loaded into memory; the exact timing is specified in the .uplugin file per-module."));
	ModuleContents.Add(TEXT("}"));
	ModuleContents.Add(FString());
	ModuleContents.Add(FString::Printf(TEXT("void F%sModule::ShutdownModule()"), *(InModuleDeclarer.ModuleName)));
	ModuleContents.Add(TEXT("{"));
	ModuleContents.Add(TEXT("\t// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,"));
	ModuleContents.Add(TEXT("\t// we call this function before unloading the module."));
	ModuleContents.Add(TEXT("}"));
	ModuleContents.Add(FString());
	switch (InModuleDeclarer.ModuleImplementType)
	{
	case EModuleImplementType::NormalModule:
		ModuleContents.Add(FString::Printf(TEXT("IMPLEMENT_MODULE(F%sModule, %s)"), *(InModuleDeclarer.ModuleName), *(InModuleDeclarer.ModuleName)));
		break;
	case EModuleImplementType::GameModule:
		ModuleContents.Add(FString::Printf(TEXT("IMPLEMENT_GAME_MODULE(F%sModule, %s)"), *(InModuleDeclarer.ModuleName), *(InModuleDeclarer.ModuleName)));
		break;
	default:
		break;
	}

	UE_LOGFMT(LogSourceModuleGenerator, Log, "Creating module source file... The expected file path is \"{Path}\"", InModuleDeclarer.ModuleSourceFilePath);
	if (!FFileHelper::SaveStringArrayToFile(ModuleContents, *(InModuleDeclarer.ModuleSourceFilePath)))
	{
		UE_LOGFMT(LogSourceModuleGenerator, Error, "Creating module source file failed.");
		::DeleteGeneratedFiles(GeneratedFilePaths);
		return false;
	}
	GeneratedFilePaths.Add(InModuleDeclarer.ModuleSourceFilePath);

	// Generate module build file.
	ModuleContents.Empty();
	ModuleContents.Add(TEXT("// ") + InModuleDeclarer.CopyrightMessage);
	ModuleContents.Add(FString());
	ModuleContents.Add(TEXT("using UnrealBuildTool;"));
	ModuleContents.Add(FString());
	ModuleContents.Add(FString::Printf(TEXT("public class %s : ModuleRules"), *(InModuleDeclarer.ModuleName)));
	ModuleContents.Add(TEXT("{"));
	ModuleContents.Add(TEXT("\t") + FString::Printf(TEXT("public %s(ReadOnlyTargetRules Target) : base(Target)"), *(InModuleDeclarer.ModuleName)));
	ModuleContents.Add(TEXT("\t{"));
	ModuleContents.Add(TEXT("\t\tPCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;"));
	ModuleContents.Add(FString());
	ModuleContents.Add(TEXT("\t\tPublicDependencyModuleNames.AddRange("));
	ModuleContents.Add(TEXT("\t\t\tnew string[]"));
	ModuleContents.Add(TEXT("\t\t\t{"));
	ModuleContents.Add(TEXT("\t\t\t\t\"Core\""));
	ModuleContents.Add(TEXT("\t\t\t\t// ... add other public dependencies that you statically link with here ..."));
	ModuleContents.Add(TEXT("\t\t\t}"));
	ModuleContents.Add(TEXT("\t\t\t);"));
	ModuleContents.Add(FString());
	ModuleContents.Add(TEXT("\t\tPrivateDependencyModuleNames.AddRange("));
	ModuleContents.Add(TEXT("\t\t\tnew string[]"));
	ModuleContents.Add(TEXT("\t\t\t{"));
	ModuleContents.Add(TEXT("\t\t\t\t\"CoreUObject\","));
	ModuleContents.Add(TEXT("\t\t\t\t\"Engine\""));
	ModuleContents.Add(TEXT("\t\t\t\t// ... add private dependencies that you statically link with here ..."));
	ModuleContents.Add(TEXT("\t\t\t}"));
	ModuleContents.Add(TEXT("\t\t\t);"));
	ModuleContents.Add(TEXT("\t}"));
	ModuleContents.Add(TEXT("}"));

	UE_LOGFMT(LogSourceModuleGenerator, Log, "Creating module build file... The expected file path is: \"{Path}\"", InModuleDeclarer.ModuleBuildFilePath);
	if (!FFileHelper::SaveStringArrayToFile(ModuleContents, *(InModuleDeclarer.ModuleBuildFilePath)))
	{
		UE_LOGFMT(LogSourceModuleGenerator, Error, "Creating module build file failed.");
		::DeleteGeneratedFiles(GeneratedFilePaths);
		return false;
	}

	for (const FString& Item : InModuleDeclarer.ProjectModuleTargetTypeFilePaths)
	{
		UE_LOGFMT(LogSourceModuleGenerator, Log, "Adding module name to project target file: \"{ItemLeft}\"... The expected file path is: \"{Item}\"", Item.Left(Item.Find(TEXT("."))), Item);
		TArray<FString> Result;
		if (!FFileHelper::LoadFileToStringArray(Result, *Item))
		{
			UE_LOGFMT(LogSourceModuleGenerator, Error, "Load project target file failed.");
			::DeleteGeneratedFiles(GeneratedFilePaths);
			return false;
		}
		bool bIsFoundKeyString = false;
		for (FString& Line : Result)
		{
			if (Line.Contains(TEXT("ExtraModuleNames.AddRange( new string[] {")))
			{
				FString LeftString, RightString;
				if (!Line.Split(TEXT("\""), &LeftString, &RightString, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
				{
					UE_LOGFMT(LogSourceModuleGenerator, Error, "Split string failed.");
					::DeleteGeneratedFiles(GeneratedFilePaths);
					return false;
				}
				Line = LeftString + TEXT("\", \"") + InModuleDeclarer.ModuleName + TEXT("\"") + RightString;
				bIsFoundKeyString = true;
			}
		}
		if (!bIsFoundKeyString)
		{
			int32 Index = 0;
			for (; Index < Result.Num(); ++Index)
			{
				int32 InsertIndex = Result[Index].Find(TEXT("base(Target)"));
				if (INDEX_NONE == InsertIndex)
				{
					continue;
				}
				else
				{
					Result.Insert(TEXT(""), Index + 2);
					Result.Insert(TEXT("\t\tExtraModuleNames.AddRange( new string[] { \"") + InModuleDeclarer.ModuleName + TEXT("\" } );"), Index + 2);
					Result.Insert(TEXT("\t\t// Add new module by Source Module Generator."), Index + 2);
					break;
				}
			}
			if (Index == Result.Num())
			{
				UE_LOGFMT(LogSourceModuleGenerator, Error, "Find key string \"base(Target)\" failed.");
				::DeleteGeneratedFiles(GeneratedFilePaths);
				return false;
			}
		}
		if (!FFileHelper::SaveStringArrayToFile(Result, *Item))
		{
			UE_LOGFMT(LogSourceModuleGenerator, Error, "Save file failed.");
			::DeleteGeneratedFiles(GeneratedFilePaths);
			return false;
		}
	}

	return true;
}

void DeleteGeneratedFiles(const TArray<FString>& InGeneratedFilePaths)
{
	UE_LOGFMT(LogSourceModuleGenerator, Log, "Deleting generated files...");
	IFileManager& FileManager = IFileManager::Get();
	for (const FString& GeneratedFilePath : InGeneratedFilePaths)
	{
		if (!FileManager.Delete(*GeneratedFilePath, true, true, false))
		{
			UE_LOGFMT(LogSourceModuleGenerator, Warning, "Deleting temporary generated file path does not success.");
		}
		UE_LOGFMT(LogSourceModuleGenerator, Log, "Deleted file %s", GeneratedFilePath);
		FString DirectoryPath = FPaths::GetPath(GeneratedFilePath);
		if (FileManager.DirectoryExists(*DirectoryPath))
		{
			FileManager.DeleteDirectory(*DirectoryPath);
			UE_LOGFMT(LogSourceModuleGenerator, Log, "Deleted directory: \"{DirPath}\"", DirectoryPath);
		}
	}
}
