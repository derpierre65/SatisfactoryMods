#pragma once

#include "CoreMinimal.h"
#include "FGBuildableFactory.h"
#include "Buildables/FGBuildable.h"

#include "EEBuildableEmcImporter.generated.h"

class AEEModSubsystem;
class UFGPowerInfoComponent;
class UFGFactoryConnectionComponent;

UCLASS()
class EQUIVALENTEXCHANGE_API AEEBuildableEmcImporter : public AFGBuildableFactory
{
	GENERATED_BODY()

public:
	AEEBuildableEmcImporter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange")
	int32 ItemsPerMinute = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange")
	float MinimumEmcValue = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange")
	float MaximumEmcValue = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFGFactoryConnectionComponent* Input0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AEEModSubsystem* EEModSubsystem;
	
	virtual void BeginPlay() override;
	virtual void Factory_Tick(float dt) override;
	virtual void Factory_CollectInput_Implementation() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Equivalent Exchange", meta=(Tooltip="0 = Unlimited"))
	int64 MaxEmcValue = 0;

private:
	float LastItemAccepted = 0;
};
