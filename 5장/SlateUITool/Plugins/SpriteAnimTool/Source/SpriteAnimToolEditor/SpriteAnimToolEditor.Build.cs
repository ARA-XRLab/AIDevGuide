using UnrealBuildTool;

public class SpriteAnimToolEditor : ModuleRules
{
	public SpriteAnimToolEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Slate",
				"SlateCore"
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AppFramework",
				"DesktopPlatform",
				"EditorFramework",
				"Json",
				"LevelEditor",
				"PropertyEditor",
				"Projects",
				"ToolMenus",
				"UnrealEd"
			});
	}
}
