// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpaceShooter : ModuleRules
{
	public SpaceShooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
