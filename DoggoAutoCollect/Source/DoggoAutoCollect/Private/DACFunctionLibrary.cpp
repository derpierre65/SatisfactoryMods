#include "DACFunctionLibrary.h"

#include "FGChatManager.h"

void UDACFunctionLibrary::BroadcastChatMessage(UObject* WorldContext, FChatMessageStruct Message)
{
	AFGChatManager::Get(WorldContext->GetWorld())->BroadcastChatMessage(Message);
}
