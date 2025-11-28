#pragma once

#include "CoreMinimal.h"
#include "FGRemoteCallObject.h"

#include "EERemoteCallObject.generated.h"

class AEEBuildableStorageEnergyCondenser;
class AEEBuildableEmcExporter;
class UFGItemDescriptor;

UCLASS()
class EQUIVALENTEXCHANGE_API UEERemoteCallObject : public UFGRemoteCallObject
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_TransferEmcToItems(TSubclassOf<UFGItemDescriptor> ItemClass, const int32 NumItems);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ChangeEmcExporterItem(AEEBuildableEmcExporter* Exporter, TSubclassOf<UFGItemDescriptor> ItemClass);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ChangeEnergyCondenserItem(AEEBuildableStorageEnergyCondenser* Condenser, TSubclassOf<UFGItemDescriptor> ItemClass);

	UFUNCTION(BlueprintImplementableEvent)
	void UnlockItem(TSubclassOf<UFGItemDescriptor> ItemClass);
};
