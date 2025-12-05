// 

#pragma once

#include "CoreMinimal.h"
#include "FGSaveInterface.h"
#include "Subsystem/ModSubsystem.h"

#include "QuestifyModSubsystem.generated.h"

class UQQuestDescriptor;

UCLASS()
class QUESTIFY_API AQuestifyModSubsystem : public AModSubsystem, public IFGSaveInterface
{
	GENERATED_BODY()

public:
	AQuestifyModSubsystem();

	static AQuestifyModSubsystem* Get(const UWorld* World);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Questify")
	void RegisterQuests(TArray<UQQuestDescriptor*> NewQuests);

	UFUNCTION(BlueprintCallable, Category = "Questify")
	void RegisterQuest(UQQuestDescriptor* NewQuest);

	UFUNCTION(BlueprintPure, Category = "Questify")
	bool IsQuestCompleted(const UQQuestDescriptor* Quest) const;

	UFUNCTION(BlueprintPure, Category = "Questify")
	bool IsQuestRevealed(const UQQuestDescriptor* Quest) const;

	UFUNCTION(BlueprintCallable, Category = "Questify")
	void SaveAllQuests() const;

	virtual bool ShouldSave_Implementation() const override;

	const TArray<UQQuestDescriptor*> GetQuests() const;
	
protected:
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Questify")
	TArray<UQQuestDescriptor*> Quests;

	UPROPERTY(BlueprintReadOnly, SaveGame, Replicated, Category = "Questify")
	TArray<UQQuestDescriptor*> CompletedQuests;
};
