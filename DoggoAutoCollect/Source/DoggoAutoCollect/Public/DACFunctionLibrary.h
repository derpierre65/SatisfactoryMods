// 

#pragma once

#include "CoreMinimal.h"
// #include "Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h"

#include "DACFunctionLibrary.generated.h"

struct FChatMessageStruct;
/**
 * 
 */
UCLASS()
class DOGGOAUTOCOLLECT_API UDACFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DoggoAutoCollect")
	static void BroadcastChatMessage(UObject* WorldContext, FChatMessageStruct Message);
};
