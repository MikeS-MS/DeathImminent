//  (C) 2025 The Hokkaido Hideout. All rights reserved.

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules
{
	public class PTMTool : ModuleRules
	{
		public PTMTool(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
			DefaultBuildSettings = BuildSettingsVersion.V3;
			PrivatePCHHeaderFile = "Public/PTMTool.h";
			IWYUSupport = IWYUSupport.KeepAsIsForNow;
			//
			PublicDependencyModuleNames.AddRange(new string[] {"Core","Engine","CoreUObject"});
			PrivateDependencyModuleNames.AddRange
			(
				new string[]
				{
					"ContentBrowser",
					"BlueprintGraph",
					"LevelEditor",
					"EditorStyle",
					"AssetTools",
					"InputCore",
					"SlateCore",
					"ToolMenus",
					"UnrealEd",
					"Projects",
					"Slate"
				}
			);
			//
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"Public"));
			PrivateIncludePaths.Add(Path.Combine(ModuleDirectory,"Private"));
		}
	}

}