#pragma once

#include "FGResearchManager.h"
#include "Subsystem/ModSubsystem.h"
#include "MoreHardDriveRerolls.generated.h"

USTRUCT(BlueprintType)
struct FRerollHardDriveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PendingRewardsRerollsExecuted = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RemainingRerolls = 0;
};

UCLASS()
class MOREHARDDRIVEREROLLS_API AMoreHardDriveRerolls : public AModSubsystem
{
	GENERATED_BODY()

public:
	AMoreHardDriveRerolls();
	
	UFUNCTION(BlueprintCallable, Category = "More Hard Drive Rerolls")
	FRerollHardDriveData GetHardDriveData(const UFGHardDrive* HardDrive);

	UFUNCTION(BlueprintCallable, Category = "More Hard Drive Rerolls")
	void UpdateMaxRerolls();
	
protected:
	virtual void BeginPlay() override;
};