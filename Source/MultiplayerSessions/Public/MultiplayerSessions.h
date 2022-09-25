#pragma once

#include "CoreMinimal.h"

class FMultiplayerSessionsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
