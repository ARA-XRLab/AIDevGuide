#include "SpriteAnimToolEditorModule.h"

#include "LevelEditor.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SSpriteAnimToolWindow.h"

#define LOCTEXT_NAMESPACE "FSpriteAnimToolEditorModule"

const FName FSpriteAnimToolEditorModule::ToolTabName(TEXT("SpriteAnimTool"));

void FSpriteAnimToolEditorModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		ToolTabName,
		FOnSpawnTab::CreateRaw(this, &FSpriteAnimToolEditorModule::SpawnToolTab))
		.SetDisplayName(LOCTEXT("ToolTabTitle", "2D Sprite Animation Tool"))
		.SetTooltipText(LOCTEXT("ToolTabTooltip", "Open the SpriteAnimTool editor window."))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSpriteAnimToolEditorModule::RegisterMenus));
}

void FSpriteAnimToolEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);

	if (UToolMenus* ToolMenus = UToolMenus::TryGet())
	{
		ToolMenus->UnregisterOwner(this);
	}

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ToolTabName);
}

void FSpriteAnimToolEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	if (UToolMenu* WindowMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window"))
	{
		FToolMenuSection& Section = WindowMenu->FindOrAddSection("WindowLayout");
		Section.AddMenuEntry(
			"OpenSpriteAnimTool",
			LOCTEXT("OpenSpriteAnimToolLabel", "Sprite Animation Tool"),
			LOCTEXT("OpenSpriteAnimToolTooltip", "Open the SpriteAnimTool window."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([]()
			{
				FGlobalTabmanager::Get()->TryInvokeTab(FSpriteAnimToolEditorModule::ToolTabName);
			})));
	}
}

TSharedRef<SDockTab> FSpriteAnimToolEditorModule::SpawnToolTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SSpriteAnimToolWindow)
		];
}

IMPLEMENT_MODULE(FSpriteAnimToolEditorModule, SpriteAnimToolEditor)

#undef LOCTEXT_NAMESPACE
