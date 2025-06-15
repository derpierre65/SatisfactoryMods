// 

#pragma once

#include "CoreMinimal.h"
#include "FGCircuitSubsystem.h"
#include "ModSubsystem.h"

#include "PSATOModSubsystem.generated.h"

class AFGBuildableCircuitSwitch;

USTRUCT()
struct POWERSWITCHAUTOTURNON_API FPSATOPriorityDetails
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<AFGBuildableCircuitSwitch*> Switches;
};

UCLASS()
class POWERSWITCHAUTOTURNON_API APSATOModSubsystem : public AModSubsystem
{
	GENERATED_BODY()

public:
	APSATOModSubsystem();

	UFUNCTION(BlueprintimplementableEvent)
	void SendSwitchOnNotification(int32 Priority, AFGBuildableCircuitSwitch* Switch);

protected:
	FTimerHandle LoopTimerHandle;

	UPROPERTY()
	TMap<int32, FPSATOPriorityDetails> PowerSwitches;

	UPROPERTY()
	TMap<int32, float> PotentialCircuitConsumption;

	UPROPERTY()
	TMap<AFGBuildableCircuitSwitch*, int64> PowerSwitchWantTurnOn;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void MainLoop();
	void GetAllPowerSwitches();
	void TurnOnSwitches(const AFGCircuitSubsystem* CircuitSubsystem);
	void TurnOnSwitch(int32 Priority, AFGBuildableCircuitSwitch* Switch);
};
