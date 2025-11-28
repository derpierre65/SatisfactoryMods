#include "EERemoteCallObject.h"

#include "EEBuildableEmcExporter.h"
#include "EEBuildableStorageEnergyCondenser.h"
#include "EEModSubsystem.h"
#include "EquivalentExchange.h"
#include "FGCharacterPlayer.h"
#include "SubsystemActorManager.h"

void UEERemoteCallObject::Server_TransferEmcToItems_Implementation(TSubclassOf<UFGItemDescriptor> ItemClass, const int32 NumItems)
{
	USubsystemActorManager* SubsystemActorManager = GetWorld()->GetSubsystem<USubsystemActorManager>();
	AEEModSubsystem* EEModSubsystem = SubsystemActorManager->GetSubsystemActor<AEEModSubsystem>();
	const int64 ItemEmcValue = EEModSubsystem->GetItemEmcValue(ItemClass);
	if (ItemEmcValue < 1)
	{
		return;
	}

	const int64 CanGenerateItems = FMath::FloorToInt64(EEModSubsystem->GetEmcValue() / ItemEmcValue);
	const int32 MaxNumItems = FMath::Min(CanGenerateItems, NumItems);
	const int64 EmcValueForNumItems = ItemEmcValue * MaxNumItems;
	if (EEModSubsystem->GetEmcValue() < EmcValueForNumItems)
	{
		return;
	}

	FInventoryStack Stack;
	Stack.Item = FInventoryItem(ItemClass);
	Stack.NumItems = MaxNumItems;
	
	const int32 AddedItems = GetOwnerPlayerCharacter()->GetInventory()->AddStack(Stack, true);

	EEModSubsystem->AddEmcValue(AddedItems * ItemEmcValue * -1);
}

void UEERemoteCallObject::Server_ChangeEmcExporterItem_Implementation(AEEBuildableEmcExporter* Exporter, TSubclassOf<UFGItemDescriptor> ItemClass)
{
	Exporter->SetItemClass(ItemClass);
}

void UEERemoteCallObject::Server_ChangeEnergyCondenserItem_Implementation(AEEBuildableStorageEnergyCondenser* Condenser, TSubclassOf<UFGItemDescriptor> ItemClass)
{
	Condenser->SetItemClass(ItemClass);
}
