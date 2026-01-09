#include "EEModSubsystem.h"

#include "EEEMCPointsData.h"
#include "EERemoteCallObject.h"
#include "EquivalentExchange.h"
#include "FGPlayerController.h"
#include "SubsystemActorManager.h"
#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AEEModSubsystem::AEEModSubsystem()
{
	PrimaryActorTick.bCanEverTick = true;
	ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnServer_Replicate;
}

AEEModSubsystem* AEEModSubsystem::Get(const UWorld* World)
{
	USubsystemActorManager* SubsystemActorManager = World->GetSubsystem<USubsystemActorManager>();

	return SubsystemActorManager->GetSubsystemActor<AEEModSubsystem>();
}

void AEEModSubsystem::BeginPlay()
{
	Super::BeginPlay();
}

void AEEModSubsystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, EmcValue);
	DOREPLIFETIME(ThisClass, EmcValues);
	DOREPLIFETIME(ThisClass, UnlockedItems);
}

void AEEModSubsystem::RegisterEmcValues(const UDataTable* EmcTable)
{
	if (!EmcTable || !HasAuthority())
	{
		return;
	}

	static const FString Context = TEXT("RegisterEmcValues");
	TArray<FEEEMCPointsData*> AllRows;
	EmcTable->GetAllRows(Context, AllRows);

	for (const FEEEMCPointsData* Row : AllRows)
	{
		SetItemEmcValue(Row->ItemClass, Row->EmcValue);
	}

	OnRep_EmcValues();
}

void AEEModSubsystem::OnRep_EmcValues()
{
	EmcValuesMapped.Reset();
	for (FEEEMCPointsData EmcEntry : EmcValues)
	{
		EmcValuesMapped.Add(EmcEntry.ItemClass, EmcEntry.EmcValue);
	}
}

void AEEModSubsystem::AddEmcValue(const int64 Value)
{
	EmcValue += Value;
}

TArray<TSubclassOf<UFGItemDescriptor>> AEEModSubsystem::GetSortedUnlockedItems() const
{
	TArray<TSubclassOf<UFGItemDescriptor>> SortedUnlockedItems = UnlockedItems;
	SortedUnlockedItems.Sort([this](const TSubclassOf<UFGItemDescriptor>& A, const TSubclassOf<UFGItemDescriptor>& B) {
		return GetItemEmcValue(A) > GetItemEmcValue(B);
	});

	return SortedUnlockedItems;
}

void AEEModSubsystem::SetItemEmcValue(const TSubclassOf<UFGItemDescriptor> Item, const int64 Value)
{
	if (EmcValuesMapped.Contains(Item))
	{
		for (int i = 0, Count = EmcValues.Num(); i < Count; ++i)
		{
			if (EmcValues[i].ItemClass == Item)
			{
				EmcValues.RemoveAt(i);
				break;
			}
			
		}
	}

	if (Value < 1)
	{
		return;
	}
	
	FEEEMCPointsData Row;
	Row.ItemClass = Item;
	Row.EmcValue = Value;

	EmcValues.Add(Row);
}

void AEEModSubsystem::UnlockItem(const TSubclassOf<UFGItemDescriptor> Item)
{
	if (UnlockedItems.Contains(Item))
	{
		return;
	}

	UnlockedItems.AddUnique(Item);
	Multicast_UnlockedItem(Item);
}

void AEEModSubsystem::Multicast_UnlockedItem_Implementation(TSubclassOf<UFGItemDescriptor> ItemClass)
{
	TWeakObjectPtr<ThisClass> WeakThis(this);
	AsyncTask(ENamedThreads::GameThread, [WeakThis, ItemClass]()
	{
		if (!WeakThis.IsValid())
		{
			return;
		}
		
		AFGPlayerController* PlayerController = Cast<AFGPlayerController>(UGameplayStatics::GetPlayerController(WeakThis->GetWorld(), 0));
		if (!PlayerController)
		{
			return;
		}

		PlayerController->GetRemoteCallObjectOfClass<UEERemoteCallObject>()->UnlockItem(ItemClass);
	});
}

int64 AEEModSubsystem::GetItemEmcValue(const TSubclassOf<UFGItemDescriptor> Item) const
{
	return EmcValuesMapped.FindRef(Item);
}

bool AEEModSubsystem::IsUnlockedItem(const TSubclassOf<UFGItemDescriptor> Item) const
{
	return UnlockedItems.Contains(Item);
}

int64 AEEModSubsystem::GetEmcValue() const
{
	return EmcValue;
}

TArray<FEEEMCPointsData> AEEModSubsystem::GetEmcValues() const
{
	return EmcValues;
}

TMap<TSubclassOf<UFGItemDescriptor>, int64> AEEModSubsystem::GetMappedEmcValues() const
{
	return EmcValuesMapped;
}

bool AEEModSubsystem::ShouldSave_Implementation() const
{
	return true;
}

void AEEModSubsystem::PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion)
{
	if (EmcValuesMapped.IsEmpty())
	{
		return;
	}

	for (TSubclassOf<UFGItemDescriptor> Item : UnlockedItems)
	{
		if (GetItemEmcValue(Item) > 0)
		{
			continue;
		}

		UE_LOG(LogEE, Log, TEXT("Removing unlocked item %s, has no emc value"), *Item->GetName());
		UnlockedItems.Remove(Item);
	}
}

void AEEModSubsystem::PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion)
{
	bIsLoaded = true;
}
