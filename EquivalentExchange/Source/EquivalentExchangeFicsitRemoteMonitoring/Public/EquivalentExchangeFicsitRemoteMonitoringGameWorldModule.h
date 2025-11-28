#pragma once

#include "CoreMinimal.h"
#include "Module/GameWorldModule.h"

#include "EquivalentExchangeFicsitRemoteMonitoringGameWorldModule.generated.h"

struct FRequestData;

UCLASS()
class EQUIVALENTEXCHANGEFICSITREMOTEMONITORING_API UEquivalentExchangeFicsitRemoteMonitoringGameWorldModule : public UGameWorldModule
{
	GENERATED_BODY()

public:
	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;
};
