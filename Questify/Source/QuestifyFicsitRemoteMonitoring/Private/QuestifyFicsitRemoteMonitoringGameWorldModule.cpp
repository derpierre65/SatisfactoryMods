#include "QuestifyFicsitRemoteMonitoringGameWorldModule.h"

#include "FGFactorySettings.h"
#include "FicsitRemoteMonitoring.h"
#include "FRM_RequestData.h"
#include "Questify/Public/QQuestDescriptor.h"
#include "Questify/Public/QuestifyModSubsystem.h"

void UQuestifyFicsitRemoteMonitoringGameWorldModule::DispatchLifecycleEvent(ELifecyclePhase Phase)
{
	Super::DispatchLifecycleEvent(Phase);

	if (Phase == ELifecyclePhase::POST_INITIALIZATION)
	{
		AFicsitRemoteMonitoring* FrmModSubsystem = AFicsitRemoteMonitoring::Get(GetWorld());
		if (IsValid(FrmModSubsystem))
		{
			FrmModSubsystem->RegisterEndpoint(FAPIEndpoint("GET", "questify/getQuests", &ThisClass::GetQuests));
		}
	}
}

void UQuestifyFicsitRemoteMonitoringGameWorldModule::GetQuests(UObject* WorldContext, FRequestData RequestData, TArray<TSharedPtr<FJsonValue>>& OutJsonArray)
{
	AQuestifyModSubsystem* QuestifyModSubsystem = AQuestifyModSubsystem::Get(WorldContext->GetWorld());
	TArray<UQQuestDescriptor*> Quests = QuestifyModSubsystem->GetQuests();
	for (const UQQuestDescriptor* Quest : Quests)
	{
		const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
		const TSharedPtr<FJsonObject> PositionJson = MakeShared<FJsonObject>();
		PositionJson->SetNumberField(TEXT("X"), Quest->Position.X);
		PositionJson->SetNumberField(TEXT("Y"), Quest->Position.Y);

		if (QuestifyModSubsystem->IsQuestRevealed(Quest))
		{
			Json->SetStringField(TEXT("Name"), Quest->Title.ToString());
			Json->SetStringField(TEXT("Description"), Quest->Description.ToString());
		}
		else
		{
			Json->SetStringField(TEXT("Name"), TEXT("Unrevealed Quest"));
			Json->SetStringField(TEXT("Description"), TEXT("Unrevealed Quest"));
		}
		
		Json->SetNumberField(TEXT("Size"), Quest->Size);
		Json->SetStringField(TEXT("Category"), UFGCategory::GetCategoryName(Quest->Category).ToString());
		Json->SetStringField(TEXT("SubCategory"), UFGCategory::GetCategoryName(Quest->SubCategory).ToString());
		Json->SetObjectField(TEXT("Position"), PositionJson);
		Json->SetBoolField(TEXT("IsCompleted"), QuestifyModSubsystem->IsQuestCompleted(Quest));
		Json->SetBoolField(TEXT("IsRevealed"), QuestifyModSubsystem->IsQuestRevealed(Quest));

		OutJsonArray.Add(MakeShared<FJsonValueObject>(Json));
	}
}
