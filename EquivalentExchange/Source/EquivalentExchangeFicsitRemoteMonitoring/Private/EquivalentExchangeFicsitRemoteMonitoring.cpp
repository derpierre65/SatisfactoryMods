// Copyright Epic Games, Inc. All Rights Reserved.

#include "EquivalentExchangeFicsitRemoteMonitoring.h"

#include "EEModSubsystem.h"
#include "NativeHookManager.h"
#include "RemoteMonitoringLibrary.h"
#include "Session.h"

#define LOCTEXT_NAMESPACE "FEquivalentExchangeFicsitRemoteMonitoring"

DEFINE_LOG_CATEGORY(LogEquivalentExchangeFicsitRemoteMonitoring);

void FEquivalentExchangeFicsitRemoteMonitoringModule::StartupModule()
{
	if (WITH_EDITOR) return;

	// cache changed World, to use it in hooks
	FWorldDelegates::OnPostWorldInitialization.AddLambda([this](UWorld* World, const UWorld::InitializationValues)
	{
		CacheWorld(World);
	});

	// try to fetch current world
	for (const FWorldContext& Ctx : GEngine->GetWorldContexts())
	{
		UWorld* World = Ctx.World();
		if (World && CacheWorld(World))
		{
			break;
		}
	}

	// add EmcValue to /getSessionInfo
	SUBSCRIBE_METHOD(
		USession::getSessionInfo,
		[](
			auto& scope,
			UObject* WorldContext,
			FRequestData RequestData,
			TArray<TSharedPtr<FJsonValue>>& OutJsonArray
		)
		{
			scope(WorldContext, RequestData, OutJsonArray);

			if (OutJsonArray.Num())
			{
				OutJsonArray[0]->AsObject()->Values.Add(TEXT("EmcValue"), MakeShared<FJsonValueNumber>(AEEModSubsystem::Get(WorldContext->GetWorld())->GetEmcValue()));
			}
		}
	);

	SUBSCRIBE_METHOD_EXPLICIT(
		TSharedPtr<FJsonObject>(*)(const TSubclassOf<UFGItemDescriptor>&, const int, float),
		URemoteMonitoringLibrary::GetItemValueObject,
		[this](
			auto& scope,
			const TSubclassOf<UFGItemDescriptor>& Item,
			const int Amount,
			float StackSizeMuliplier
		)
		{
			TSharedPtr<FJsonObject> JItem = scope(Item, Amount, StackSizeMuliplier);
			
			if (JItem.IsValid() && IsValid(CachedWorld))
            {
				int32 ItemAmount = JItem->GetIntegerField(TEXT("Amount"));
				const int64 EmcValue = AEEModSubsystem::Get(CachedWorld)->GetItemEmcValue(Item);
            	JItem->SetNumberField(TEXT("EmcValue"), EmcValue);
				JItem->SetNumberField(TEXT("TotalEmcValue"), EmcValue * Amount);
            }
			
			return JItem;
		}
	);
}

void FEquivalentExchangeFicsitRemoteMonitoringModule::ShutdownModule()
{
}

bool FEquivalentExchangeFicsitRemoteMonitoringModule::CacheWorld(UWorld* World)
{
	if (World && (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE))
	{
		CachedWorld = World;

		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEquivalentExchangeFicsitRemoteMonitoringModule, EquivalentExchangeFicsitRemoteMonitoring)