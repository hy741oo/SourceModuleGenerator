// Copyright 2022 U.N.Owen, All Rights Reserved.

#include "ModuleFile.h"
#include "ModuleDeclarer.h"
#include "SourceModuleGenerator.h"

bool CreateModuleFiles(const FModuleDeclarer& InModuleDeclarer)
{
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

	if (!FFileHelper::SaveStringArrayToFile(ModuleContents, *(InModuleDeclarer.ModulePublicPath)))
	{
		UE_LOG(LogSourceModuleGenerator, Error, TEXT("Create module header file failed!"));
		return false;
	}

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

	if (!FFileHelper::SaveStringArrayToFile(ModuleContents, *(InModuleDeclarer.ModulePrivatePath)))
	{
		UE_LOG(LogSourceModuleGenerator, Error, TEXT("Create module source file failed!"));
		return false;
	}

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
	ModuleContents.Add(FString());
	ModuleContents.Add(TEXT("\t\t\t\t// ... add private dependencies that you statically link with here ..."));
	ModuleContents.Add(TEXT("\t\t\t}"));
	ModuleContents.Add(TEXT("\t\t\t);"));
	ModuleContents.Add(TEXT("\t}"));
	ModuleContents.Add(TEXT("}"));

	if (!FFileHelper::SaveStringArrayToFile(ModuleContents, *(InModuleDeclarer.ModuleRootPath)))
	{
		UE_LOG(LogSourceModuleGenerator, Error, TEXT("Create module build file failed!"));
		return false;
	}

	return true;
}
