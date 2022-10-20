// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DediServerTest : ModuleRules
{
	public DediServerTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities", 
            "GameplayTags", 
            "GameplayTasks",
            "Core", 
            "CoreUObject", 
            "Engine", 
            "AIModule",
            "HeadMountedDisplay", 
            "ModularGameplay"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "EnhancedInput",
            "InputCore",
            "CommonUI",
            "CommonInput",
        });

    }
}
