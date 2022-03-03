// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "ModuleFile.h"
#include "ModuleDeclarer.h"
#include "SourceModuleGenerator.h"
#include "ProjectDescriptor.h"
#include "PluginDescriptor.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "HAL/FileManager.h"

#include <cstdio>

// Internal function declares.
void DeleteGeneratedFiles(const TArray<FString>& InGeneratedFilePaths);

bool CreateModule(const FModuleDeclarer& InModuleDeclarer)
{
	// Add module descriptor to project or plugin descriptor.
	UE_LOG(LogSourceModuleGenerator, Log, TEXT("Adding module descriptor to descriptor file... The expected file path is %s"), *(InModuleDeclarer.DescriptorFilePath));
	if (!(IFileManager::Get().FileExists(*(InModuleDeclarer.DescriptorFilePath))))
	{
		UE_LOG(LogSourceModuleGenerator, Error, TEXT("Descriptor file does not exist."));
		return false;
	}

	FILE* DescriptorFileHandle = std::fopen(TCHAR_TO_UTF8(*(InModuleDeclarer.DescriptorFilePath)), "rb");
	char ContentBuffer[65536] = { 0 };
	rapidjson::FileReadStream DescriptorFileHandleReadStream(DescriptorFileHandle, ContentBuffer, sizeof(ContentBuffer));
	rapidjson::Document JsonDocument;
	JsonDocument.ParseStream(DescriptorFileHandleReadStream);
	std::fclose(DescriptorFileHandle);

	if (!JsonDocument.IsObject())
	{
		UE_LOG(LogSourceModuleGenerator, Error, TEXT("Load descriptor file failed."));
		return false;
	}

	rapidjson::Value ModuleDescriptor;
	ModuleDescriptor.SetObject();
	ModuleDescriptor.AddMember("Name", rapidjson::StringRef(TCHAR_TO_UTF8(*(InModuleDeclarer.ModuleName))), JsonDocument.GetAllocator());
	ModuleDescriptor.AddMember("Type", rapidjson::StringRef(TCHAR_TO_UTF8(*(InModuleDeclarer.HostType))), JsonDocument.GetAllocator());
	ModuleDescriptor.AddMember("LoadingPhase", rapidjson::StringRef(TCHAR_TO_UTF8(*(InModuleDeclarer.LoadingPhase))), JsonDocument.GetAllocator());

	if (JsonDocument.HasMember("Modules"))
	{
		rapidjson::Value& ModuleList = JsonDocument["Modules"];
		for (rapidjson::Value& ModuleObject : ModuleList.GetArray())
		{
			if (ModuleObject.HasMember("Name"))
			{
				if (ModuleObject["Name"] == TCHAR_TO_UTF8(*(InModuleDeclarer.ModuleName)))
				{
					UE_LOG(LogSourceModuleGenerator, Error, TEXT("Module %s has already been added to Descriptor file, Abort generating."));
					return false;
				}
			}
		}
		ModuleList.PushBack(ModuleDescriptor, JsonDocument.GetAllocator());
	}
	else
	{
		rapidjson::Value ModuleList;
		ModuleList.SetArray();
		ModuleList.PushBack(ModuleDescriptor, JsonDocument.GetAllocator());
		JsonDocument.AddMember("Modules", ModuleList, JsonDocument.GetAllocator());
	}

	DescriptorFileHandle = std::fopen(TCHAR_TO_UTF8(*(InModuleDeclarer.DescriptorFilePath)), "wb");
	rapidjson::FileWriteStream DescriptorFileWriteStream(DescriptorFileHandle, ContentBuffer, sizeof(ContentBuffer));

	rapidjson::PrettyWriter<rapidjson::FileWriteStream> JsonWriter(DescriptorFileWriteStream);
	JsonWriter.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
	JsonWriter.SetIndent('\t', 1);

	if (!JsonDocument.Accept(JsonWriter))
	{
		UE_LOG(LogSourceModuleGenerator, Error, TEXT("Add module descriptor failed."));
		return false;
	}
	std::fclose(DescriptorFileHandle);

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

	UE_LOG(LogSourceModuleGenerator, Log, TEXT("Creating module header file... The expected file path is %s"), *(InModuleDeclarer.ModuleHeaderFilePath));
	if (!FFileHelper::SaveStringArrayToFile(ModuleContents, *(InModuleDeclarer.ModuleHeaderFilePath)))
	{
		UE_LOG(LogSourceModuleGenerator, Error, TEXT("Creating module header file failed."));
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

	UE_LOG(LogSourceModuleGenerator, Log, TEXT("Creating module source file... The expected file path is %s"), *(InModuleDeclarer.ModuleSourceFilePath));
	if (!FFileHelper::SaveStringArrayToFile(ModuleContents, *(InModuleDeclarer.ModuleSourceFilePath)))
	{
		UE_LOG(LogSourceModuleGenerator, Error, TEXT("Creating module source file failed."));
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

	UE_LOG(LogSourceModuleGenerator, Log, TEXT("Creating module build file... The expected file path is %s"), *(InModuleDeclarer.ModuleBuildFilePath));
	if (!FFileHelper::SaveStringArrayToFile(ModuleContents, *(InModuleDeclarer.ModuleBuildFilePath)))
	{
		UE_LOG(LogSourceModuleGenerator, Error, TEXT("Creating module build file failed."));
		::DeleteGeneratedFiles(GeneratedFilePaths);
		return false;
	}

	return true;
}

void DeleteGeneratedFiles(const TArray<FString>& InGeneratedFilePaths)
{
	UE_LOG(LogSourceModuleGenerator, Log, TEXT("Deleting generated files..."));
	IFileManager& FileManager = IFileManager::Get();
	for (const FString& GeneratedFilePath : InGeneratedFilePaths)
	{
		if (!FileManager.Delete(*GeneratedFilePath, true, true, false))
		{
			UE_LOG(LogSourceModuleGenerator, Warning, TEXT("Deleting temporary generated file path does not success."));
		}
		UE_LOG(LogSourceModuleGenerator, Log, TEXT("Deleted file %s"), *GeneratedFilePath);
		FString DirectoryPath = FPaths::GetPath(GeneratedFilePath);
		if (FileManager.DirectoryExists(*DirectoryPath))
		{
			FileManager.DeleteDirectory(*DirectoryPath);
			UE_LOG(LogSourceModuleGenerator, Log, TEXT("Deleted directory %s"), *DirectoryPath);
		}
	}
}
