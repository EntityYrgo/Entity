using UnrealBuildTool;
using System.IO;
 
public class AdvancedSessions : ModuleRulesBase
{
    public AdvancedSessions(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        //bEnforceIWYU = true;

        PublicDefinitions.Add("WITH_ADVANCED_SESSIONS=1");

        PrivateIncludePaths.AddRange(new string[] { "AdvancedSessions/Private" });
        PublicIncludePaths.AddRange(new string[] { "AdvancedSessions/Public" });
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "Networking", "Sockets" });

        // Uncomment the following line if you are using Steam as your online subsystem
        PrivateDependencyModuleNames.Add("OnlineSubsystemSteam");
    }
}
