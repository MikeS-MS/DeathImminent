// Copyright 2022 RLoris

#pragma once

#include "Modules/ModuleManager.h"

class FFileHelperModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
