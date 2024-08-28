// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class UnrealFlecs : ModuleRules
{
	public UnrealFlecs(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
		bUseUnity = false;
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"StructUtils",
			"Json",
			"JsonUtilities"
		});

		//The path for the header files
		PublicIncludePaths.AddRange(new string[] {Path.Combine(ModuleDirectory, "Public")});
		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {Path.Combine(ModuleDirectory, "Private")});

		if (Target.bBuildEditor)
		{
			PrivateDefinitions.Add("flecs_EXPORTS");
		}
		else
		{
			PublicDefinitions.Add("flecs_EXPORTS");
		}
		
		PublicDefinitions.Add("FLECS_CPP_NO_AUTO_REGISTRATION = 1");
		
		
		if (Target.Configuration < UnrealTargetConfiguration.Test)
		{
			PublicDefinitions.Add("FLECS_SOFT_ASSERT = 1");
		}

		if (Target.Configuration < UnrealTargetConfiguration.Development)
		{ 
			// A cheesy way to turn off NDEBUG for Flecs
			PublicDefinitions.Add("DUMMYDEFINE\n#ifdef NDEBUG\n#undef NDEBUG\n#endif");
			PublicDefinitions.Add("FLECS_DEBUG = 1");
			PublicDefinitions.Add("FLECS_SANITIZE = 1");
		}
	}
}
