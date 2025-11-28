#include "EquivalentExchangeFicsitRemoteMonitoringGameWorldModule.h"

#include "EquivalentExchangeFicsitRemoteMonitoring.h"
#include "FicsitRemoteMonitoring.h"

void UEquivalentExchangeFicsitRemoteMonitoringGameWorldModule::DispatchLifecycleEvent(ELifecyclePhase Phase)
{
	Super::DispatchLifecycleEvent(Phase);

	if (Phase == ELifecyclePhase::POST_INITIALIZATION)
	{
		AFicsitRemoteMonitoring* FrmModSubsystem = AFicsitRemoteMonitoring::Get(GetWorld());
		if (IsValid(FrmModSubsystem))
		{
			UE_LOG(LogEquivalentExchangeFicsitRemoteMonitoring, Log, TEXT("register endpoints?"));
		}
	}
}
