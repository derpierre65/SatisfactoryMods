#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EEEMCPointsData.generated.h"

USTRUCT(BlueprintType)
struct FEEEMCPointsData : public FTableRowBase
{
	GENERATED_BODY()

	FEEEMCPointsData() : ItemClass(nullptr), EmcValue(0) {}

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSubclassOf<class UFGItemDescriptor> ItemClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int64 EmcValue;
};