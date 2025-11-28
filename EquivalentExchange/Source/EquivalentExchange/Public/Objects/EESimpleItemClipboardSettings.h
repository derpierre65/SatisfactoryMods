#pragma once

#include "CoreMinimal.h"
#include "FGFactoryClipboard.h"

#include "EESimpleItemClipboardSettings.generated.h"

class UFGItemDescriptor;

UCLASS()
class EQUIVALENTEXCHANGE_API UEESimpleItemClipboardSettings : public UFGFactoryClipboardSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UFGItemDescriptor> ItemClass;
};
