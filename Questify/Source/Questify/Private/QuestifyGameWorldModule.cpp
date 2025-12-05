#include "QuestifyGameWorldModule.h"

#include "Questify.h"
#include "Misc/CoreDelegates.h"
#include "ModLoading/PluginModuleLoader.h"

TFuture<IModuleInterface*> UQuestifyGameWorldModule::LoadQuestify(const FName ModName)
{
	auto& ModuleManager = FModuleManager::Get();
	const FName QuestifySubModName = FName(TEXT("Questify") + ModName.ToString());

	TPromise<IModuleInterface*> Promise;

	if (ModuleManager.IsModuleLoaded(ModName))
	{
		Promise.SetValue(ModuleManager.LoadModule(QuestifySubModName));
	}
	else
	{
		FDelegateHandle LoadHandle;
		LoadHandle = ModuleManager.OnModulesChanged().AddLambda(
			[&ModName, &QuestifySubModName, &Promise, &ModuleManager, &LoadHandle](FName Name, EModuleChangeReason Reason)
			{
				if (Name != ModName)
				{
					return;
				}

				if (Reason == EModuleChangeReason::ModuleLoaded)
				{
					ModuleManager.OnModulesChanged().Remove(LoadHandle);
					Promise.SetValue(ModuleManager.LoadModule(QuestifySubModName));
				}
			});
	}

	return Promise.GetFuture();
}

void UQuestifyGameWorldModule::LoadFicsitRemoteMonitoring()
{
	LoadQuestify(FName(TEXT("FicsitRemoteMonitoring"))).Then([this](TFuture<IModuleInterface*> ModuleInterface)
	{
		IModuleInterface* SubModule = ModuleInterface.Get();
		if (!SubModule)
		{
			UE_LOG(LogQuestify, Error, TEXT("Failed to load QuestifyFicsitRemoteMonitoring"));
			return;
		}

		UClass* test = LoadClass<UModModule>(nullptr, TEXT("/Script/QuestifyFicsitRemoteMonitoring.QuestifyFicsitRemoteMonitoringGameWorldModule"));
		if (!test)
		{
			UE_LOG(LogQuestify, Error, TEXT("Can't load class :("));
			return;
		}

		UModModule* ChildModule = SpawnChildModule(FName(TEXT("BBT_FicsitFarming_CDOs")), TSoftClassPtr<UModModule>(test));

		if (!IsValid(ChildModule))
		{
			UE_LOG(LogQuestify, Error, TEXT("Failed to spawn BBT_FicsitFarming_CDOs"));
			return;
		}

		UE_LOG(LogQuestify, Display, TEXT("Applied FicsitFarming Tweaks"));
	});
}
