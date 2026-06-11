#pragma once

#include "Async/Future.h"
#include "CoreMinimal.h"
#include "Module/GameWorldModule.h"

#include "EEGameWorldModule.generated.h"

class IModuleInterface;

USTRUCT(BlueprintType)
struct EQUIVALENTEXCHANGE_API FEEModDependency
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ModName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Module to load when ModName is present. Leave empty to use the default: <YourModName><ModName>"))
	FName LoadModuleName;

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
	TArray<FEEModDependency> ExternalModModules;

private:
	TFuture<IModuleInterface*> LoadModDependency(const FEEModDependency &Dependency) const;
	static bool IsModLoaded(const FName& ModName, const IModuleInterface* SubModule);

	void OnModDependencyLoaded(const FEEModDependency& Dependency, const IModuleInterface* SubModule);
};