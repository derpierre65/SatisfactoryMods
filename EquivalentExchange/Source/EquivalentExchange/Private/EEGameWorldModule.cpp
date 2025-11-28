#include "EEGameWorldModule.h"

#include "EquivalentExchange.h"
#include "Misc/CoreDelegates.h"
#include "ModLoading/PluginModuleLoader.h"

TFuture<IModuleInterface*> UEEGameWorldModule::LoadModDependency(const FName ModName)
{
	auto& ModuleManager = FModuleManager::Get();
	const FName ModDependencyName = FName(TEXT("EquivalentExchange") + ModName.ToString());

	TPromise<IModuleInterface*> Promise;

	if (ModuleManager.IsModuleLoaded(ModName))
	{
		Promise.SetValue(ModuleManager.LoadModule(ModDependencyName));
	}
	else
	{
		FDelegateHandle LoadHandle;
		LoadHandle = ModuleManager.OnModulesChanged().AddLambda([
				&ModName,
				&ModDependencyName,
				&Promise,
				&ModuleManager,
				&LoadHandle
			](const FName Name, const EModuleChangeReason Reason)
			{
				if (Name != ModName)
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
	for (FEEModDependency ExternalModModule : ExternalModModules)
	{
		LoadModDependency(ExternalModModule.ModName).Then([this, &ExternalModModule](TFuture<IModuleInterface*> ModuleInterface)
			{
				const IModuleInterface* SubModule = ModuleInterface.Get();
				if (!SubModule)
				{
					UE_LOG(LogEE, Error, TEXT("Can't load mod dependency for %s."), *ExternalModModule.ModName.ToString());
					return;
				}

				FString BaseModuleName = FString::Printf(TEXT("%s_Dep_%s"), *ModName, *ExternalModModule.ModName.ToString());
				int32 Dependency = 0;

				for (FString CppModule : ExternalModModule.CppModules)
				{
					UClass* CppClass = LoadClass<UModModule>(nullptr, *CppModule);
					if (!CppClass)
					{
						UE_LOG(LogEE, Error, TEXT("Can't load mod dependency class for %s (%s)."), *ExternalModModule.ModName.ToString(), *CppModule);
						continue;
					}

					Dependency++;
					const UModModule* ChildModule = SpawnChildModule(FName(FString::Printf(TEXT("%s_Cpp_%d"), *BaseModuleName, Dependency)), TSoftClassPtr<UModModule>(CppClass));
					if (!IsValid(ChildModule))
					{
						UE_LOG(LogEE, Error, TEXT("Can't spawn mod dependency module for %s."), *ExternalModModule.ModName.ToString());
					}
				}

				for (TSubclassOf<UModModule> Module : ExternalModModule.Modules)
				{
					Dependency++;
					const UModModule* ChildModule = SpawnChildModule(FName(FString::Printf(TEXT("%s_Module_%d"), *BaseModuleName, Dependency)), TSoftClassPtr<UModModule>(Module));
					if (!IsValid(ChildModule))
					{
						UE_LOG(LogEE, Error, TEXT("Can't spawn mod dependency module for %s (%s)."), *ExternalModModule.ModName.ToString(), *Module->GetPathName());
					}
				}
			});		
	}
}
