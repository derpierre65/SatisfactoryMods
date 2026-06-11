#pragma once

#include "CoreMinimal.h"
#include "Module/GameWorldModule.h"

#include "EquivalentExchangeFRMGameWorldModule.generated.h"

struct FRequestData;

UCLASS()
class EQUIVALENTEXCHANGEFRM_API UEquivalentExchangeFRMGameWorldModule : public UGameWorldModule
{
	GENERATED_BODY()

public:
	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;
};
