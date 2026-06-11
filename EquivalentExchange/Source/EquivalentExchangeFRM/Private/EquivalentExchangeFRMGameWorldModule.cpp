#include "EquivalentExchangeFRMGameWorldModule.h"

#include "EquivalentExchangeFRM.h"
#include "FicsitRemoteMonitoring.h"

void UEquivalentExchangeFRMGameWorldModule::DispatchLifecycleEvent(ELifecyclePhase Phase)
{
	Super::DispatchLifecycleEvent(Phase);

	if (Phase == ELifecyclePhase::POST_INITIALIZATION)
	{
		AFicsitRemoteMonitoring* FrmModSubsystem = AFicsitRemoteMonitoring::Get(GetWorld());
		if (IsValid(FrmModSubsystem))
		{
			// register new endpoints here
		}
	}
}
