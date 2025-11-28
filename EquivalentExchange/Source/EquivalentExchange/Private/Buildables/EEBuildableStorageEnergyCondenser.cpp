#include "EEBuildableStorageEnergyCondenser.h"

#include "EEModSubsystem.h"
#include "EquivalentExchange.h"
#include "FGFactoryConnectionComponent.h"
#include "UnrealNetwork.h"
#include "Objects/EESimpleItemClipboardSettings.h"

AEEBuildableStorageEnergyCondenser::AEEBuildableStorageEnergyCondenser()
{
	PrimaryActorTick.bCanEverTick = true;
	OutputInventory = CreateDefaultSubobject<UFGInventoryComponent>(TEXT("Output Inventory"));
	PreviewSlotInventory = CreateDefaultSubobject<UFGInventoryComponent>(TEXT("Preview Slot Inventory"));
}

void AEEBuildableStorageEnergyCondenser::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	OutputInventory->Resize(mInventorySizeX * mInventorySizeY);
	PreviewSlotInventory->Resize(3);
	PreviewSlotInventory->SetLocked(true);
	SetItemClass(ItemClass);

	PreviewSlotInventory->SetReplicationRelevancyOwner(this);
	OutputInventory->SetReplicationRelevancyOwner(this);
	GetStorageInventory()->SetReplicationRelevancyOwner(this);

	for (UFGFactoryConnectionComponent* FactoryConnection : GetConnectionComponents())
	{
		if (FactoryConnection->GetDirection() == EFactoryConnectionDirection::FCD_INPUT)
		{
			FactoryConnection->SetInventory(GetStorageInventory());
		}
		else if (FactoryConnection->GetDirection() == EFactoryConnectionDirection::FCD_OUTPUT)
		{
			FactoryConnection->SetInventory(OutputInventory);
		}
	}

	EEModSubsystem = AEEModSubsystem::Get(GetWorld());
}

void AEEBuildableStorageEnergyCondenser::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemClass);
}

void AEEBuildableStorageEnergyCondenser::GetConditionalReplicatedProps(TArray<FFGCondReplicatedProperty>& outProps) const
{
	Super::GetConditionalReplicatedProps(outProps);

	FG_DOREPCONDITIONAL(ThisClass, InternalEmcValue);
}

void AEEBuildableStorageEnergyCondenser::Factory_Tick(float dt)
{
	Super::Factory_Tick(dt);

	if (!EEModSubsystem || !ItemClass || !HasAuthority())
	{
		return;
	}

	// TODO workaround for now, 
	// PreviewSlotInventory->SetAllowedItemOnIndex(0, ItemClass);

	bool bEmcChanged = false;
	int32 ProcessedSlots = 0;
	for (int32 Slot = mInventorySizeX * mInventorySizeY - 1; Slot >= 0; Slot--)
	{
		FInventoryStack InventoryStack;
		GetStorageInventory()->GetStackFromIndex(Slot, InventoryStack);

		const TSubclassOf<UFGItemDescriptor> SlotItem = InventoryStack.Item.GetItemClass();
		if (!SlotItem)
		{
			continue;
		}

		const int64 EmcValue = EEModSubsystem->GetItemEmcValue(SlotItem);
		if (EmcValue <= 0)
		{
			continue;
		}

		const int32 ProcessItems = FMath::Min(MaxItemsPerSlot, InventoryStack.NumItems);

		GetStorageInventory()->Remove(SlotItem, ProcessItems);
		InternalEmcValue += EmcValue * ProcessItems;
		ProcessedSlots++;
		bEmcChanged = true;

		if (ProcessedSlots >= MaxSlotsPerTick)
		{
			break;
		}
	}

	const int64 RequiredEmc = EEModSubsystem->GetItemEmcValue(ItemClass);
	const int32 GenerateItems = FMath::Min(ProduceMaxItemsPerTick, FMath::FloorToInt64(InternalEmcValue / RequiredEmc));
	const int32 AddedItems = OutputInventory->AddStack(FInventoryStack(GenerateItems, ItemClass), true);
	if (InternalEmcValue >= RequiredEmc && AddedItems >= 0)
	{
		InternalEmcValue -= RequiredEmc * AddedItems;
		bEmcChanged = true;
	}

	if (bEmcChanged)
	{
		mPropertyReplicator.MarkPropertyDirty(FName("InternalEmcValue"));
	}
}

void AEEBuildableStorageEnergyCondenser::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion)
{
	Super::PostLoadGame_Implementation(saveVersion, gameVersion);

	SetItemClass(ItemClass);

	FTimerHandle TimerUpdateItem;
	TWeakObjectPtr<ThisClass> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(TimerUpdateItem, [WeakThis]()
	{
		if (!WeakThis.IsValid())
		{
			return;
		}

		WeakThis->SetItemClass(WeakThis->ItemClass);
	}, 1.f, false);
}

void AEEBuildableStorageEnergyCondenser::SetItemClass(const TSubclassOf<UFGItemDescriptor> NewItemClass)
{
	ItemClass = NewItemClass;

	OutputInventory->SetLocked(ItemClass == nullptr);
	PreviewSlotInventory->SetAllowedItemOnIndex(0, NewItemClass);
	for (int32 Slot = mInventorySizeX * mInventorySizeY - 1; Slot >= 0; Slot--)
	{
		OutputInventory->SetAllowedItemOnIndex(Slot, NewItemClass);
	}
}

UFGFactoryClipboardSettings* AEEBuildableStorageEnergyCondenser::CopySettings_Implementation()
{
	UEESimpleItemClipboardSettings* Settings = NewObject<UEESimpleItemClipboardSettings>(this);
	Settings->ItemClass = ItemClass;

	return Settings;
}

bool AEEBuildableStorageEnergyCondenser::PasteSettings_Implementation(UFGFactoryClipboardSettings* factoryClipboard, AFGPlayerController* player)
{
	const UEESimpleItemClipboardSettings* Settings = Cast<UEESimpleItemClipboardSettings>(factoryClipboard);
	if (!Settings || !EEModSubsystem->IsUnlockedItem(Settings->ItemClass))
	{
		return false;
	}

	SetItemClass(Settings->ItemClass);

	return true;
}

bool AEEBuildableStorageEnergyCondenser::CanUseFactoryClipboard_Implementation()
{
	return true;
}

TSubclassOf<UObject> AEEBuildableStorageEnergyCondenser::GetClipboardMappingClass_Implementation()
{
	return UEESimpleItemClipboardSettings::StaticClass();
}
