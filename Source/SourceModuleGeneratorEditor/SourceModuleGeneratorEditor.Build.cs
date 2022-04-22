// Copyright 2022 U.N.Owen, All Rights Reserved.

using UnrealBuildTool;

public class SourceModuleGeneratorEditor : ModuleRules
{
	public SourceModuleGeneratorEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
				// ... add other public dependencies that you statically link with here ...
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"MainFrame",
				"InputCore",
				"Projects",
				"SourceModuleGenerator",
				"GameProjectGeneration",
				"HotReload"
				// ... add private dependencies that you statically link with here ...
			}
			);
	}
}
