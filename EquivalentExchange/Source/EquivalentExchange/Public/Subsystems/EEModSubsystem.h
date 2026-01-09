#pragma once

#include "CoreMinimal.h"
#include "FGItemDescriptor.h"
#include "FGSaveInterface.h"
#include "Subsystem/ModSubsystem.h"
#include "EEEMCPointsData.h"

#include "EEModSubsystem.generated.h"

struct FEEEMCPointsData;

UCLASS()
class EQUIVALENTEXCHANGE_API AEEModSubsystem : public AModSubsystem, public IFGSaveInterface
{
	GENERATED_BODY()

public:
	AEEModSubsystem();

	static AEEModSubsystem* Get(const UWorld* World);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool ShouldSave_Implementation() const override;
	virtual void PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion) override;
	virtual void PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion) override;

	UFUNCTION(BlueprintCallable)
	void RegisterEmcValues(const UDataTable* EmcTable);

	UFUNCTION(BlueprintPure)
	int64 GetEmcValue() const;

	UFUNCTION(BlueprintPure)
	TArray<FEEEMCPointsData> GetEmcValues() const;

	UFUNCTION(BlueprintPure)
	TMap<TSubclassOf<UFGItemDescriptor>, int64> GetMappedEmcValues() const;

	UFUNCTION(BlueprintCallable)
	int64 GetItemEmcValue(const TSubclassOf<UFGItemDescriptor> Item) const;

	UFUNCTION(BlueprintPure)
	bool IsUnlockedItem(const TSubclassOf<UFGItemDescriptor> Item) const;
	
	void SetItemEmcValue(const TSubclassOf<UFGItemDescriptor> Item, const int64 Value);

	UFUNCTION(BlueprintCallable)
	void UnlockItem(const TSubclassOf<UFGItemDescriptor> Item);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UnlockedItem(TSubclassOf<UFGItemDescriptor> ItemClass);
	
	UFUNCTION()
	void OnRep_EmcValues();

	UFUNCTION(BlueprintCallable)
	void AddEmcValue(const int64 Value);

	UFUNCTION(BlueprintPure)
	TArray<TSubclassOf<UFGItemDescriptor>> GetSortedUnlockedItems() const;
	
protected:
	UPROPERTY(Replicated, SaveGame, BlueprintReadOnly)
	TArray<TSubclassOf<UFGItemDescriptor>> UnlockedItems;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	bool bIsLoaded = false;

	UPROPERTY(Replicated, SaveGame)
	int64 EmcValue = 0;

	UPROPERTY(ReplicatedUsing = OnRep_EmcValues)
	TArray<FEEEMCPointsData> EmcValues;

	UPROPERTY()
	TMap<TSubclassOf<UFGItemDescriptor>, int64> EmcValuesMapped;

	virtual void BeginPlay() override;
};
