// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AIBattleSystem : ModuleRules
{
	public AIBattleSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AIModule", "GameplayTasks" });

        PublicIncludePaths.AddRange(new string[] { "AIBattleSystem", "AIBattleSystem/Public" });

        PrivateIncludePaths.AddRange(new string[] { "AIBattleSystem/Private" });
    }
}
