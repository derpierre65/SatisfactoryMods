#pragma once
#include "CoreMinimal.h"
#include "Configuration/ConfigManager.h"
#include "Engine/Engine.h"
#include "TrainAlert_ConfigStruct.generated.h"

/* Struct generated from Mod Configuration Asset '/TrainAlert/TrainAlert_Config' */
USTRUCT(BlueprintType)
struct FTrainAlert_ConfigStruct {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    int32 TrainStopDuration{};

    UPROPERTY(BlueprintReadWrite)
    float NotificationDuration{};

    UPROPERTY(BlueprintReadWrite)
    int32 LoopTime{};

    /* Retrieves active configuration value and returns object of this struct containing it */
    static FTrainAlert_ConfigStruct GetActiveConfig(UObject* WorldContext) {
        FTrainAlert_ConfigStruct ConfigStruct{};
        FConfigId ConfigId{"TrainAlert", ""};
        if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull)) {
            UConfigManager* ConfigManager = World->GetGameInstance()->GetSubsystem<UConfigManager>();
            ConfigManager->FillConfigurationStruct(ConfigId, FDynamicStructInfo{FTrainAlert_ConfigStruct::StaticStruct(), &ConfigStruct});
        }
        return ConfigStruct;
    }
};

