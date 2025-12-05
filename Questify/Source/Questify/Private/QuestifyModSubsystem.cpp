#include "QuestifyModSubsystem.h"

#include "QQuestDescriptor.h"
#include "Questify.h"
#include "SubsystemActorManager.h"
#include "UnrealNetwork.h"

AQuestifyModSubsystem::AQuestifyModSubsystem()
{
	PrimaryActorTick.bCanEverTick = true;
	ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnServer_Replicate;
}

AQuestifyModSubsystem* AQuestifyModSubsystem::Get(const UWorld* World)
{
	USubsystemActorManager* SubsystemActorManager = World->GetSubsystem<USubsystemActorManager>();

	return SubsystemActorManager->GetSubsystemActor<AQuestifyModSubsystem>();
}

void AQuestifyModSubsystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Quests);
}
//
// void AQuestifyModSubsystem::BeginPlay()
// {
// 	Super::BeginPlay();

// }

void AQuestifyModSubsystem::RegisterQuests(const TArray<UQQuestDescriptor*> NewQuests)
{
	Quests.Append(NewQuests);
}

void AQuestifyModSubsystem::RegisterQuest(UQQuestDescriptor* NewQuest)
{
	Quests.Add(NewQuest);
}

bool AQuestifyModSubsystem::IsQuestCompleted(const UQQuestDescriptor* Quest) const
{
	return CompletedQuests.Contains(Quest);
}

bool AQuestifyModSubsystem::IsQuestRevealed(const UQQuestDescriptor* Quest) const
{
	if (Quest->PrerequisitesToReveal.IsEmpty())
	{
		return true;
	}

	for (const UQQuestDescriptor* PrerequisiteQuest : Quest->PrerequisitesToReveal)
	{
		if (!IsQuestCompleted(PrerequisiteQuest))
		{
			return false;
		}
	}

	return true;
}

void AQuestifyModSubsystem::SaveAllQuests() const
{
	for (UQQuestDescriptor* Quest : Quests)
	{
		FString Filename = FPaths::MakeValidFileName(FString::Printf(TEXT("%s"), *Quest->Title.ToString()));
		FString ModFilePath = FString::Printf(TEXT("%sMods/Questify/Export/%s.json"), *FPaths::ProjectDir(), *Filename);
		FFileHelper::SaveStringToFile(Quest->ToJson(), *ModFilePath);
	}
}

bool AQuestifyModSubsystem::ShouldSave_Implementation() const
{
	return true;
}

const TArray<UQQuestDescriptor*> AQuestifyModSubsystem::GetQuests() const
{
	return Quests;
}
