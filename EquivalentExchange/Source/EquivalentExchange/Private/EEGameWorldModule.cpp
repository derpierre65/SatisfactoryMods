#include "EEGameWorldModule.h"

#include "Async/Future.h"
#include "EquivalentExchange.h"
#include "Misc/CoreDelegates.h"
#include "ModLoading/PluginModuleLoader.h"
#include "GameFeaturesSubsystem.h"

TFuture<IModuleInterface *> UEEGameWorldModule::LoadModDependency(const FEEModDependency &Dependency) const
{
	auto& ModuleManager = FModuleManager::Get();
	const FName ModDependencyName = Dependency.LoadModuleName.IsNone() ? FName(GetOwnerModReference().ToString() + Dependency.ModName.ToString()) : Dependency.LoadModuleName;
	
	TPromise<IModuleInterface*> Promise;
	if (ModuleManager.IsModuleLoaded(Dependency.ModName))
	{
		Promise.SetValue(ModuleManager.LoadModule(ModDependencyName));
	}
	else
	{
		FDelegateHandle LoadHandle;
		LoadHandle = ModuleManager.OnModulesChanged().AddLambda([
				&Dependency,
				&ModDependencyName,
				&Promise,
				&ModuleManager,
				&LoadHandle
			](const FName Name, const EModuleChangeReason Reason)
			{
				if (Name != Dependency.ModName)
				{
					return;
				}

				if (Reason == EModuleChangeReason::ModuleLoaded)
				{
					ModuleManager.OnModulesChanged().Remove(LoadHandle);
					Promise.SetValue(ModuleManager.LoadModule(ModDependencyName));
				}
			});
	}

	return Promise.GetFuture();
}

void UEEGameWorldModule::LoadModDependencies()
{
	for (const FEEModDependency& ExternalModModule : ExternalModModules)
	{
		LoadModDependency(ExternalModModule).Then([this, ExternalModModule](TFuture<IModuleInterface*> ModuleInterface)
		{
			OnModDependencyLoaded(ExternalModModule, ModuleInterface.Get());
		});
	}
}

void UEEGameWorldModule::OnModDependencyLoaded(const FEEModDependency& Dependency, const IModuleInterface* SubModule)
{
	if (!IsModLoaded(Dependency.ModName, SubModule))
	{
		UE_LOG(LogEE, Warning, TEXT("Mod %s not loaded."), *Dependency.ModName.ToString());
		return;
	}

	FString BaseModuleName = FString::Printf(TEXT("%s_Dep_%s"), *GetOwnerModReference().ToString(), *Dependency.ModName.ToString());
	int32 Index = 0;

	for (FString CppModule : Dependency.CppModules)
	{
		UClass* CppClass = LoadClass<UModModule>(nullptr, *CppModule);
		if (!CppClass)
		{
			UE_LOG(LogEE, Error, TEXT("Can't load mod dependency class for %s (%s)."), *Dependency.ModName.ToString(), *CppModule);
			continue;
		}

		Index++;
		const UModModule* ChildModule = SpawnChildModule(FName(FString::Printf(TEXT("%s_Cpp_%d"), *BaseModuleName, Index)), TSoftClassPtr<UModModule>(CppClass));
		if (!IsValid(ChildModule))
		{
			UE_LOG(LogEE, Error, TEXT("Can't spawn mod dependency module for %s."), *Dependency.ModName.ToString());
		}
		else
		{
			UE_LOG(LogEE, Log, TEXT("Spawned CppModule %s."), *Dependency.ModName.ToString());
		}
	}

	for (TSubclassOf<UModModule> Module : Dependency.Modules)
	{
		Index++;
		const UModModule* ChildModule = SpawnChildModule(FName(FString::Printf(TEXT("%s_Module_%d"), *BaseModuleName, Index)), TSoftClassPtr<UModModule>(Module));
		if (!IsValid(ChildModule))
		{
			UE_LOG(LogEE, Error, TEXT("Can't spawn mod dependency module for %s (%s)."), *Dependency.ModName.ToString(), *Module->GetPathName());
		}
		else
		{
			UE_LOG(LogEE, Log, TEXT("Spawned Module %s."), *Dependency.ModName.ToString());
		}
	}
}

bool UEEGameWorldModule::IsModLoaded(const FName& ModName, const IModuleInterface* SubModule) {
	FString URL;
	if (UGameFeaturesSubsystem::Get().GetPluginURLByName(ModName.ToString(), URL))
	{
		return true;
	}

	return SubModule != nullptr;
}