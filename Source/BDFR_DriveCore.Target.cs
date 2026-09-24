using UnrealBuildTool;
using System.Collections.Generic;

public class BDFR_DriveCoreTarget : TargetRules
{
    public BDFR_DriveCoreTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("BDFR_DriveCore");
    }
}
