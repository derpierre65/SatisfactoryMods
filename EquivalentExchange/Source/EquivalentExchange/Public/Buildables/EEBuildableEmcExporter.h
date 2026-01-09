// 

#pragma once

#include "CoreMinimal.h"
#include "FGBuildableFactory.h"

#include "EEBuildableEmcExporter.generated.h"

class AEEModSubsystem;

UCLASS()
class EQUIVALENTEXCHANGE_API AEEBuildableEmcExporter : public AFGBuildableFactory
{
	GENERATED_BODY()

public:
	AEEBuildableEmcExporter();

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void GetConditionalReplicatedProps(TArray<FFGCondReplicatedProperty>& outProps) const override;
	virtual void Factory_Tick(float dt) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFGFactoryConnectionComponent* OutputConnection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFGInventoryComponent* OutputInventory;

	void SetItemClass(const TSubclassOf<UFGItemDescriptor> NewItemClass);

	virtual void PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion) override;
	
	// IFGFactoryClipboardInterface
	virtual UFGFactoryClipboardSettings* CopySettings_Implementation() override;
	virtual bool PasteSettings_Implementation(UFGFactoryClipboardSettings* factoryClipboard, class AFGPlayerController* player) override;
	virtual bool CanUseFactoryClipboard_Implementation() override;
	virtual TSubclassOf<UObject> GetClipboardMappingClass_Implementation() override;

protected:
	void SetLastGrabbedTime(const float Time);
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, SaveGame)
	TSubclassOf<UFGItemDescriptor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeToProduceItem;
	
	UPROPERTY(Transient)
	AEEModSubsystem* EEModSubsystem;

	UPROPERTY(BlueprintReadOnly, meta=(FGReplicated))
	float LastGrabbedTime = 0.f;
};
