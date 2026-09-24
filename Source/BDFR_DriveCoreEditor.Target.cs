using UnrealBuildTool;
using System.Collections.Generic;

public class BDFR_DriveCoreEditorTarget : TargetRules
{
    public BDFR_DriveCoreEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("BDFR_DriveCore");
    }
}
