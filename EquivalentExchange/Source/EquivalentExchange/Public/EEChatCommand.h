// 

#pragma once

#include "CoreMinimal.h"
#include "Command/ChatCommandInstance.h"

#include "EEChatCommand.generated.h"

class UFGItemDescriptor;

UCLASS()
class EQUIVALENTEXCHANGE_API AEEChatCommand : public AChatCommandInstance
{
	GENERATED_BODY()

public:
	AEEChatCommand();

	virtual EExecutionStatus ExecuteCommand_Implementation(UCommandSender* Sender, const TArray<FString>& Arguments, const FString& Label) override;

private:
	static void SendItemNotFound(UCommandSender* Sender, const FString& ItemName, TArray<TSubclassOf<UFGItemDescriptor>>& ItemDescriptors);
	static TArray<FString> SplitItemName(const FString& Name);
};
