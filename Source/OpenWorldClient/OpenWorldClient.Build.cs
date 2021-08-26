// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OpenWorldClient : ModuleRules
{
	public OpenWorldClient(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG",  "Json", "JsonUtilities" });

		PrivateDependencyModuleNames.AddRange(new string[] {"Slate", "SlateCore", "Sockets", "Networking" });
	}
}
