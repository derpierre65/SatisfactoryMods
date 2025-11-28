#pragma once

#include "CoreMinimal.h"
#include "Module/GameWorldModule.h"

#include "EEGameWorldModule.generated.h"

USTRUCT(BlueprintType)
struct EQUIVALENTEXCHANGE_API FEEModDependency
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ModName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UModModule>> Modules;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> CppModules;
};

UCLASS()
class EQUIVALENTEXCHANGE_API UEEGameWorldModule : public UGameWorldModule
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void LoadModDependencies();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ModName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEEModDependency> ExternalModModules;

private:
	TFuture<IModuleInterface*> LoadModDependency(FName ModName);
};
