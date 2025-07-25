using UnrealBuildTool;

public class FlecsInventory : ModuleRules
{
    public FlecsInventory(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;
        CppStandard = CppStandardVersion.Cpp20;

        PublicDependencyModuleNames.AddRange(
            [
                "Core",
                "CoreUObject",
                "Engine",
                "GameplayTags",
                "UnrealFlecs",
                "Json",
                "JsonUtilities"
            ]
        );
    }
}