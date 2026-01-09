// 

#pragma once

#include "CoreMinimal.h"
#include "FGBuildableStorage.h"

#include "EEBuildableStorageEnergyCondenser.generated.h"

class AEEModSubsystem;

UCLASS()
class EQUIVALENTEXCHANGE_API AEEBuildableStorageEnergyCondenser : public AFGBuildableStorage
{
	GENERATED_BODY()

public:
	AEEBuildableStorageEnergyCondenser();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GetConditionalReplicatedProps(TArray<FFGCondReplicatedProperty>& outProps) const override;
	virtual void BeginPlay() override;
	virtual void Factory_Tick(float dt) override;

	virtual void PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion) override;

	void SetItemClass(const TSubclassOf<UFGItemDescriptor> NewItemClass);

	// IFGFactoryClipboardInterface
	virtual UFGFactoryClipboardSettings* CopySettings_Implementation() override;
	virtual bool PasteSettings_Implementation(UFGFactoryClipboardSettings* factoryClipboard, AFGPlayerController* player) override;
	virtual bool CanUseFactoryClipboard_Implementation() override;
	virtual TSubclassOf<UObject> GetClipboardMappingClass_Implementation() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange|Consume")
	int32 MaxSlotsPerTick = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange|Consume")
	int32 MaxItemsPerSlot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange|Produce")
	int32 ProduceMaxItemsPerTick = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange|Consume")
	int32 ConsumeItemsEveryTicks = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange|Consume")
	bool bStopConsumeItemsIfOutputIsFull = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange|Consume")
	bool bStopConsumeItemsIfEnoughEMC = false;

	UPROPERTY(BlueprintReadOnly)
	AEEModSubsystem* EEModSubsystem;
	
	UPROPERTY()
	UFGFactoryConnectionComponent* InputConnection;

	UPROPERTY()
	UFGFactoryConnectionComponent* OutputConnection;
	
	UPROPERTY(BlueprintReadOnly)
	UFGInventoryComponent* OutputInventory;

	UPROPERTY(BlueprintReadOnly)
	UFGInventoryComponent* PreviewSlotInventory;
	
	UPROPERTY(BlueprintReadOnly, Replicated, SaveGame)
	TSubclassOf<UFGItemDescriptor> ItemClass;

	UPROPERTY(BlueprintReadOnly, SaveGame, meta=(FGReplicated))
	int64 InternalEmcValue = 0;
	
	int32 ConsumeItemTicks = 0;
};
