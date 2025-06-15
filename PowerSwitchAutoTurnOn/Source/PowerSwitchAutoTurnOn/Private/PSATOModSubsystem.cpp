// 

#include "PSATOModSubsystem.h"

#include "FGBuildableCircuitSwitch.h"
#include "FGBuildablePriorityPowerSwitch.h"
#include "FGBuildableSubsystem.h"
#include "FGCircuitConnectionComponent.h"
#include "FGCircuitSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#define PSATO_DEBUG false

// Sets default values
APSATOModSubsystem::APSATOModSubsystem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APSATOModSubsystem::BeginPlay()
{
	Super::BeginPlay();

	// ignore timer for none servers
	if (!UKismetSystemLibrary::IsDedicatedServer(GetWorld()) && !UKismetSystemLibrary::IsServer(GetWorld())) return;

	GetWorld()->GetTimerManager().SetTimer(LoopTimerHandle, this, &ThisClass::MainLoop, 2.f, true);
}

void APSATOModSubsystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(LoopTimerHandle);
}

void APSATOModSubsystem::MainLoop()
{
	AFGCircuitSubsystem* CircuitSubsystem = AFGCircuitSubsystem::Get(GetWorld());

#if PSATO_DEBUG
	UE_LOG(LogCore, Log, TEXT("------------"));
	UE_LOG(LogCore, Log, TEXT("Get All Power Switches"));
#endif
	// fetch all priority power switches
	GetAllPowerSwitches();

	// check which switches should be turned on automatically
	PotentialCircuitConsumption.Reset();
#if PSATO_DEBUG
		UE_LOG(LogCore, Log, TEXT(""));
		UE_LOG(LogCore, Log, TEXT(""));
		UE_LOG(LogCore, Log, TEXT(""));
		UE_LOG(LogCore, Log, TEXT("Turn on required power switches:"));
#endif
	TurnOnSwitches(CircuitSubsystem);

#if PSATO_DEBUG
	UE_LOG(LogCore, Log, TEXT("------------"));
#endif
}

void APSATOModSubsystem::GetAllPowerSwitches()
{
	AFGBuildableSubsystem* BuildableSubsystem = AFGBuildableSubsystem::Get(GetWorld());
	TArray<AFGBuildableCircuitSwitch*> PowerSwitchBuildables;
	BuildableSubsystem->GetTypedBuildable<AFGBuildableCircuitSwitch>(PowerSwitchBuildables);

	PowerSwitches.Reset();
	int32 FoundSwitches = 0;
	for (AFGBuildableCircuitSwitch* PowerSwitch : PowerSwitchBuildables)
	{
		// this power switch has not 2 connections
		if (!PowerSwitch->IsSwitchConnected())
		{
#if PSATO_DEBUG
			UE_LOG(LogCore, Warning, TEXT("Ignore power switch %s (a or b is not connected)"), *PowerSwitch->GetBuildingTag_Implementation());
#endif

			continue;
		}

		// get priority
		int Priority = -1; // normal power switch
		const AFGBuildablePriorityPowerSwitch* PriorityPowerSwitch = Cast<AFGBuildablePriorityPowerSwitch>(PowerSwitch);
		if (PriorityPowerSwitch)
		{
			Priority = PriorityPowerSwitch->GetPriority();
		}

		// ignore priority power switches without a priority
		if (Priority == 0)
		{
#if PSATO_DEBUG
			UE_LOG(LogCore, Warning, TEXT("Ignore power switch %s (no priority available)"), *PowerSwitch->GetBuildingTag_Implementation());
#endif

			continue;
		}

		// add power switch
		if (!PowerSwitches.Contains(Priority)) PowerSwitches.Add(Priority, FPSATOPriorityDetails());

		PowerSwitches[Priority].Switches.Add(PowerSwitch);
		FoundSwitches++;

#if PSATO_DEBUG
		UE_LOG(LogCore, Log, TEXT("Found Switch %s"), *PowerSwitch->GetBuildingTag_Implementation())
#endif
	}

	PowerSwitches.KeySort([](const int32& A, const int32& B)
	{
		return A < B;
	});

#if PSATO_DEBUG
	UE_LOG(LogCore, Log, TEXT("found %d switches"), FoundSwitches);
#endif
}

void APSATOModSubsystem::TurnOnSwitches(const AFGCircuitSubsystem* CircuitSubsystem)
{
	int64 ServerTime = FMath::RoundToInt64(UGameplayStatics::GetTimeSeconds(GetWorld()));
	
	for (TPair<int, FPSATOPriorityDetails>& PriorityDetails : PowerSwitches)
	{
		for (AFGBuildableCircuitSwitch* Switch : PriorityDetails.Value.Switches)
		{
			if (Switch->IsSwitchOn())
			{
				PowerSwitchWantTurnOn.Remove(Switch);
#if PSATO_DEBUG
				UE_LOG(LogCore, Log, TEXT("power switch is on, remove from PowerSwitchWantTurnOn"));
#endif
				continue;
			}

#if PSATO_DEBUG
			UE_LOG(LogCore, Log, TEXT("-----"));
			UE_LOG(LogCore, Log, TEXT("Switch: %s"), *Switch->GetBuildingTag_Implementation());
#endif

			const int32 MainCircuitID = Switch->GetCircuitID0();

#if PSATO_DEBUG
			UE_LOG(LogCore, Log, TEXT("Circuit ID A: %d"), MainCircuitID);
			UE_LOG(LogCore, Log, TEXT("Circuit ID B: %d"), Switch->GetCircuitID1());
#endif
			
			const UFGPowerCircuit* CircuitA = Cast<UFGPowerCircuit>(CircuitSubsystem->FindCircuit(MainCircuitID));
			const UFGPowerCircuit* CircuitB = Cast<UFGPowerCircuit>(CircuitSubsystem->FindCircuit(Switch->GetCircuitID1()));

			const float CircuitAProductionCapacity = CircuitA->GetPowerProductionCapacity();
			const float UnusedCapacity = CircuitAProductionCapacity - CircuitA->mPowerConsumed;
			const float MaxPowerConsumptionB = CircuitB->GetMaximumPowerConsumption();

#if PSATO_DEBUG
				UE_LOG(LogCore, Log, TEXT("CircuitAProductionCapacity: %f"), CircuitAProductionCapacity);
				UE_LOG(LogCore, Log, TEXT("UnusedCapacity: %f"), UnusedCapacity);
				UE_LOG(LogCore, Log, TEXT("MaxPowerConsumptionB: %f"), MaxPowerConsumptionB);
#endif
			
			// circuit A has not enough capacity for this power switch
			if (UnusedCapacity < MaxPowerConsumptionB)
			{
				PowerSwitchWantTurnOn.Remove(Switch);
#if PSATO_DEBUG
				UE_LOG(LogCore, Log, TEXT("not stable enough, don't turn on 1"));
#endif

				continue;
			}

			const float PotentialConsumptionA = PotentialCircuitConsumption.Contains(MainCircuitID) ? PotentialCircuitConsumption[MainCircuitID] : 0;
			const float MainCircuitConsumptionAfterTurnOn = MaxPowerConsumptionB + PotentialConsumptionA;
			const bool bShouldTurnOn = UnusedCapacity > MainCircuitConsumptionAfterTurnOn;

#if PSATO_DEBUG
			UE_LOG(LogCore, Log, TEXT("Potential Consumption for circuit A: %f"), PotentialConsumptionA);
			UE_LOG(LogCore, Log, TEXT("MainCircuitConsumptionAfterTurnOn: %f"), MainCircuitConsumptionAfterTurnOn);
			UE_LOG(LogCore, Log, TEXT("bShouldTurnOn: %d"), bShouldTurnOn);
#endif

			if (bShouldTurnOn)
			{
				if ( PotentialCircuitConsumption.Contains(MainCircuitID) )
				{
					PotentialCircuitConsumption[MainCircuitID] += MaxPowerConsumptionB;
#if PSATO_DEBUG
					UE_LOG(LogCore, Log, TEXT("Increase %d PotentialCircuitConsumption to %f"), MainCircuitID, PotentialCircuitConsumption[MainCircuitID]);
#endif
				}
				else
				{
					PotentialCircuitConsumption.Add(MainCircuitID, MaxPowerConsumptionB);
#if PSATO_DEBUG
					UE_LOG(LogCore, Log, TEXT("%d PotentialCircuitConsumption not found, set to %f"), MainCircuitID, PotentialCircuitConsumption[MainCircuitID]);
#endif
				}
			}

			if (bShouldTurnOn)
			{
				if (!PowerSwitchWantTurnOn.Contains(Switch))
				{
					PowerSwitchWantTurnOn.Add(Switch, ServerTime + 60);
#if PSATO_DEBUG
					UE_LOG(LogCore, Log, TEXT("turn on in 60s if stable enough"));
#endif
				}
				else if (PowerSwitchWantTurnOn[Switch] < ServerTime)
				{
#if PSATO_DEBUG
					UE_LOG(LogCore, Log, TEXT("stable enough to turn on"));
#endif
					TurnOnSwitch(PriorityDetails.Key, Switch);
					PowerSwitchWantTurnOn.Remove(Switch);
				}
			}
			else
			{
#if PSATO_DEBUG
				UE_LOG(LogCore, Log, TEXT("not stable enough, don't turn on 2"));
#endif
				PowerSwitchWantTurnOn.Remove(Switch);
			}
		}
	}
}

void APSATOModSubsystem::TurnOnSwitch(int32 Priority, AFGBuildableCircuitSwitch* Switch)
{
	Switch->SetSwitchOn(true);
	SendSwitchOnNotification(Priority, Switch);
}
