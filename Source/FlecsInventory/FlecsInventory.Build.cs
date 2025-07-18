using UnrealBuildTool;

public class FlecsInventory : ModuleRules
{
    public FlecsInventory(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;
        CppStandard = CppStandardVersion.Latest;

        PublicDependencyModuleNames.AddRange(
            [
                "Core",
                "GameplayTags",
                "UnrealFlecs"
            ]
        );

        PrivateDependencyModuleNames.AddRange(
            [
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Json",
                "JsonUtilities",
            ]
        );
    }
}