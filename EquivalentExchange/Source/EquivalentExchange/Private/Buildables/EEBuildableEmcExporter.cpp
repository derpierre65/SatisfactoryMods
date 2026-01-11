#include "Buildables/EEBuildableEmcExporter.h"

#include "EEModSubsystem.h"
#include "EESimpleItemClipboardSettings.h"
#include "EquivalentExchange.h"
#include "FGFactoryConnectionComponent.h"
#include "FGPowerInfoComponent.h"
#include "UnrealNetwork.h"

AEEBuildableEmcExporter::AEEBuildableEmcExporter()
{
	PrimaryActorTick.bCanEverTick = true;

	OutputConnection = CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Output0"));
	OutputConnection->SetDirection(EFactoryConnectionDirection::FCD_OUTPUT);
	OutputConnection->SetupAttachment(RootComponent);

	OutputInventory = CreateDefaultSubobject<UFGInventoryComponent>(TEXT("Output Inventory"));
}

void AEEBuildableEmcExporter::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	EEModSubsystem = AEEModSubsystem::Get(GetWorld());
	OutputInventory->Resize(1);
	OutputInventory->SetReplicationRelevancyOwner(this);
	OutputInventory->SetAllowedItemOnIndex(0, ItemClass);
}

void AEEBuildableEmcExporter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	mPowerConsumption = 5.f;
	mPowerInfo->SetMaximumTargetConsumption(10.f);
	OutputConnection->SetInventory(OutputInventory);
	OutputConnection->SetInventoryAccessIndex(0);
}

void AEEBuildableEmcExporter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemClass);
}

void AEEBuildableEmcExporter::GetConditionalReplicatedProps(TArray<FFGCondReplicatedProperty>& outProps) const
{
	Super::GetConditionalReplicatedProps(outProps);

	FG_DOREPCONDITIONAL(ThisClass, LastGrabbedTime);
}

void AEEBuildableEmcExporter::Factory_Tick(float dt)
{
	Super::Factory_Tick(dt);

	if (!HasAuthority())
	{
		return;
	}
	
	if (!ItemClass || !OutputConnection->IsConnected() || !HasPower() || !OutputInventory->IsEmpty())
	{
		SetLastGrabbedTime(0.f);
		return;
	}

	const int64 RequiredEmc = EEModSubsystem->GetItemEmcValue(ItemClass);
	if (RequiredEmc < 1 || EEModSubsystem->GetEmcValue() < RequiredEmc)
	{
		SetLastGrabbedTime(0.f);
		return;
	}

	float NewGrabbedTime = LastGrabbedTime + dt;
	if (NewGrabbedTime >= TimeToProduceItem)
	{
		NewGrabbedTime -= TimeToProduceItem;
		if (OutputInventory->IsEmpty())
		{
			OutputInventory->AddItem(FInventoryItem(ItemClass));
			EEModSubsystem->AddEmcValue(-RequiredEmc);
		}
	}

	SetLastGrabbedTime(NewGrabbedTime);
}

void AEEBuildableEmcExporter::SetItemClass(const TSubclassOf<UFGItemDescriptor> NewItemClass)
{
	// not unlocked item or same itme
	if (!EEModSubsystem->IsUnlockedItem(NewItemClass) || NewItemClass == ItemClass)
	{
		return;
	}

	// item is not selectable, it is higher than MaxEmcValue
	const int64 NewEmcValue = EEModSubsystem->GetItemEmcValue(NewItemClass);
	if (MaxEmcValue > 0 && NewEmcValue > MaxEmcValue)
	{
		return;
	}

	ItemClass = NewItemClass;
	SetLastGrabbedTime(0.f);

	if (!OutputInventory->IsEmpty())
	{
		FInventoryStack InventoryStack;
		OutputInventory->GetStackFromIndex(0, InventoryStack);
		EEModSubsystem->AddEmcValue(InventoryStack.NumItems * EEModSubsystem->GetItemEmcValue(InventoryStack.Item.GetItemClass()));
		OutputInventory->Empty();
	}

	OutputInventory->SetAllowedItemOnIndex(0, NewItemClass);
}

void AEEBuildableEmcExporter::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion)
{
	if (ItemClass != nullptr && EEModSubsystem->GetItemEmcValue(ItemClass) < 1)
	{
		SetItemClass(nullptr);
		UE_LOG(LogEE, Log, TEXT("Reset exporter (%s) item, item has no emc value."), *GetName());
	}
	
	Super::PreSaveGame_Implementation(saveVersion, gameVersion);
}

void AEEBuildableEmcExporter::SetLastGrabbedTime(const float Time)
{
	LastGrabbedTime = Time;
	mPropertyReplicator.MarkPropertyDirty(FName("LastGrabbedTime"));
}

UFGFactoryClipboardSettings* AEEBuildableEmcExporter::CopySettings_Implementation()
{
	UEESimpleItemClipboardSettings* Settings = NewObject<UEESimpleItemClipboardSettings>(this);
	Settings->ItemClass = ItemClass;

	return Settings;
}

bool AEEBuildableEmcExporter::PasteSettings_Implementation(UFGFactoryClipboardSettings* factoryClipboard, AFGPlayerController* player)
{
	const UEESimpleItemClipboardSettings* Settings = Cast<UEESimpleItemClipboardSettings>(factoryClipboard);
	if (!Settings || !EEModSubsystem->IsUnlockedItem(Settings->ItemClass))
	{
		return false;
	}

	SetItemClass(Settings->ItemClass);

	return true;
}

bool AEEBuildableEmcExporter::CanUseFactoryClipboard_Implementation()
{
	return true;
}

TSubclassOf<UObject> AEEBuildableEmcExporter::GetClipboardMappingClass_Implementation()
{
	return UEESimpleItemClipboardSettings::StaticClass();
}