#include "Buildables/EEBuildableEmcImporter.h"

#include "EEModSubsystem.h"
#include "FGFactoryConnectionComponent.h"

AEEBuildableEmcImporter::AEEBuildableEmcImporter()
{
	Input0 = CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Input0"));
	Input0->SetupAttachment(RootComponent);
}

void AEEBuildableEmcImporter::BeginPlay()
{
	Super::BeginPlay();

	EEModSubsystem = AEEModSubsystem::Get(GetWorld());
}

void AEEBuildableEmcImporter::Factory_Tick(float dt)
{
	Super::Factory_Tick(dt);

	LastItemAccepted += dt;
}

void AEEBuildableEmcImporter::Factory_CollectInput_Implementation()
{
	if (!HasAuthority() || !Input0->IsConnected() || !HasPower())
	{
		LastItemAccepted = 60.f / ItemsPerMinute;
		return;
	}

	if (LastItemAccepted < 60.f / ItemsPerMinute)
	{
		return;
	}

	TArray<FInventoryItem> OutputItems;
	Input0->Factory_PeekOutput(OutputItems);
	if (!OutputItems.Num())
	{
		LastItemAccepted = 60.f / ItemsPerMinute;
		return;
	}

	int64 Value = EEModSubsystem->GetItemEmcValue(OutputItems[0].GetItemClass());
	if (Value < 1)
	{
		LastItemAccepted = 60.f / ItemsPerMinute;
		return;
	}

	FInventoryItem Item;
	float Offset;
	if (Input0->Factory_GrabOutput(Item, Offset, nullptr))
	{
		const float Percent = FMath::RandRange(MinimumEmcValue, MaximumEmcValue);
		Value = FMath::Max(1, FMath::FloorToInt64(Value * (Percent / 100.f)));
		EEModSubsystem->AddEmcValue(Value);
		EEModSubsystem->UnlockItem(Item.GetItemClass());
		LastItemAccepted = FMath::Max(0.f, LastItemAccepted - 60.f / ItemsPerMinute);
	}
}
