// Copyright Epic Games, Inc. All Rights Reserved.

using System.Collections.Generic;
using UnrealBuildTool;

public class witch_ue5 : ModuleRules
{
	public witch_ue5(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"Niagara", 
			"AIModule",
			"NavigationSystem",
			"GameplayTasks",
			"UMG",
			"Chaos",
            "GeometryCollectionEngine",
            "LevelSequence",
			"MovieScene",
            "Slate",
			"SlateCore",
			"ApplicationCore"
        });

		PublicIncludePaths.AddRange(
		new string[] {
			"witch_ue5"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
