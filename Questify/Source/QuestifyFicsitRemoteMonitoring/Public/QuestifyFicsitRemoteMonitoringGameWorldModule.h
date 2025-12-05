#pragma once

#include "CoreMinimal.h"
#include "Module/GameWorldModule.h"

#include "QuestifyFicsitRemoteMonitoringGameWorldModule.generated.h"

struct FRequestData;

UCLASS()
class QUESTIFYFICSITREMOTEMONITORING_API UQuestifyFicsitRemoteMonitoringGameWorldModule : public UGameWorldModule
{
	GENERATED_BODY()

public:
	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;
	
private:
	static void GetQuests(UObject* WorldContext, FRequestData RequestData, TArray<TSharedPtr<FJsonValue>>& OutJsonArray);
};
