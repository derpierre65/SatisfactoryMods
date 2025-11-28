// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEquivalentExchangeFicsitRemoteMonitoring, Log, All);

class FEquivalentExchangeFicsitRemoteMonitoringModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	UPROPERTY()
	UWorld* CachedWorld;
};
