#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSpriteAnimToolEditorModule : public IModuleInterface
{
public:
	static const FName ToolTabName;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();
	TSharedRef<class SDockTab> SpawnToolTab(const class FSpawnTabArgs& SpawnTabArgs);
};
