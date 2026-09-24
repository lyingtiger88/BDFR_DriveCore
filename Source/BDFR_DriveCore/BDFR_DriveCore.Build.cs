using UnrealBuildTool;

public class BDFR_DriveCore : ModuleRules
{
    public BDFR_DriveCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "ChaosVehicles", "BDFRPhysicsCore" });
    }
}
