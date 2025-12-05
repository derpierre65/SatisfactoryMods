#pragma once

#include "CoreMinimal.h"
#include "Module/GameWorldModule.h"

#include "QuestifyGameWorldModule.generated.h"

UCLASS()
class QUESTIFY_API UQuestifyGameWorldModule : public UGameWorldModule
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void LoadFicsitRemoteMonitoring();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UModModule> LoadModuleClass;
	
private:
	TFuture<IModuleInterface*> LoadQuestify(FName ModName);
};
