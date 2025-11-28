#include "EEChatCommand.h"

#include "CommandSender.h"
#include "EEModSubsystem.h"
#include "EquivalentExchange.h"
#include "FGBlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AEEChatCommand::AEEChatCommand()
{
	PrimaryActorTick.bCanEverTick = false;

	CommandName = "ee";
	Usage = FText::FromString("/ee - Displays the help for Equivalent Exchange.");
}

EExecutionStatus AEEChatCommand::ExecuteCommand_Implementation(UCommandSender* Sender, const TArray<FString>& Arguments, const FString& Label)
{
	const int32 NumArguments = Arguments.Num();

	AEEModSubsystem* EEModSubsystem = AEEModSubsystem::Get(GetWorld());

	if (NumArguments >= 1 && Arguments[0] == "emc")
	{
		if (NumArguments < 2)
		{
			Sender->SendChatMessage("Use:\n/ee emc set <ItemName> <NewValue>\n/ee emc del <ItemName>\n/ee emc export", FLinearColor::White);

			return EExecutionStatus::COMPLETED;
		}

		if (Arguments[1] == "export")
		{
			FString ExportString = "---,ItemClass,EmcValue\n";
			TMap<TSubclassOf<UFGItemDescriptor>, int64> EmcValues = EEModSubsystem->GetMappedEmcValues();

			for (TPair<TSubclassOf<UFGItemDescriptor>, int64>& EmcEntry : EmcValues)
			{
				const FString ItemClassName = UKismetSystemLibrary::GetClassDisplayName(EmcEntry.Key);
				FString NormalItemName = ItemClassName;
				if (ItemClassName.StartsWith(TEXT("Desc_")))
				{
					NormalItemName = ItemClassName.Mid(5);
				}
				if (ItemClassName.EndsWith(TEXT("_C")))
				{
					NormalItemName = NormalItemName.Mid(0, NormalItemName.Len() - 2);
				}

				ExportString.Append(FString::Printf(TEXT("\"%s\",\"%s\",%lld\n"), *NormalItemName, *UKismetSystemLibrary::GetPathName(EmcEntry.Key), EmcEntry.Value));

				UE_LOG(LogEE, Log, TEXT("%s || %s || %s || %s"),
					*UKismetSystemLibrary::GetSoftClassPath(EmcEntry.Key).ToString(),
					*UKismetSystemLibrary::GetPathName(EmcEntry.Key),
					*EmcEntry.Key->GetClassPathName().ToString(),
					*EmcEntry.Key->GetPathName()
				);
			}

			// /Game/FactoryGame/Resource/Parts/Rotor/Desc_Rotor.

			const FString ModFilePath = TEXT("Mods/EquivalentExchange/Export/EmcValues.csv");
			FString Filename = FPaths::ProjectDir() + ModFilePath;
			FFileHelper::SaveStringToFile(ExportString, *Filename);

			Sender->SendChatMessage(FString::Printf(TEXT("Exported %d entries to %s."), EmcValues.Num(), *ModFilePath), FLinearColor::Green);

			return EExecutionStatus::COMPLETED;
		}

		TSubclassOf<UFGItemDescriptor> ItemDescriptor = nullptr;
		TArray<TSubclassOf<UFGItemDescriptor>> ItemDescriptors;
		FString ItemName;
		if ((Arguments[1] == "set" && NumArguments >= 4) || (Arguments[1] == "del" && NumArguments >= 3))
		{
			UFGBlueprintFunctionLibrary::Cheat_GetAllDescriptors(ItemDescriptors);
			ItemName = Arguments[2].ToLower();

			for (TSubclassOf<UFGItemDescriptor> Descriptor : ItemDescriptors)
			{
				if (UFGItemDescriptor::GetItemName(Descriptor).ToString().Replace(TEXT(" "), TEXT("")).ToLower() == ItemName)
				{
					ItemDescriptor = Descriptor;
					break;
				}
			}
		}

		// /ee emc set ReinforcedIronPlate 1234
		// emc[0] set[1] itemname[2] value[3]
		if (Arguments[1] == "set")
		{
			if (NumArguments < 4)
			{
				Sender->SendChatMessage("Use: /ee emc set <ItemName> <NewValue>", FLinearColor::White);

				return EExecutionStatus::COMPLETED;
			}

			if (!ItemDescriptor)
			{
				SendItemNotFound(Sender, ItemName, ItemDescriptors);

				return EExecutionStatus::COMPLETED;
			}

			const int64 Value = FCString::Atoi(*Arguments[3]);
			if (Value < 1)
			{
				Sender->SendChatMessage(FString::Printf(TEXT("Please set a minimum EMC value of 1.")), FLinearColor::Red);

				return EExecutionStatus::COMPLETED;
			}

			EEModSubsystem->SetItemEmcValue(ItemDescriptor, Value);
			EEModSubsystem->OnRep_EmcValues();
			Sender->SendChatMessage(
				FString::Printf(TEXT("Updated EMC value for %s to %lld."), *UFGItemDescriptor::GetItemName(ItemDescriptor).ToString(), Value),
				FLinearColor::White
			);

			return EExecutionStatus::COMPLETED;
		}

		// /ee emc del ReinforcedIronPlate
		// emc[0] del[1] itemname[2]
		if (Arguments[1] == "del")
		{
			if (!ItemDescriptor)
			{
				SendItemNotFound(Sender, ItemName, ItemDescriptors);

				return EExecutionStatus::COMPLETED;
			}

			EEModSubsystem->SetItemEmcValue(ItemDescriptor, 0);
			EEModSubsystem->OnRep_EmcValues();

			Sender->SendChatMessage(
				FString::Printf(TEXT("Deleted EMC value for %s."), *UFGItemDescriptor::GetItemName(ItemDescriptor).ToString()),
				FLinearColor::White
			);

			return EExecutionStatus::COMPLETED;
		}
	}

	Sender->SendChatMessage("Command not found, use:\n/ee emc set <ItemName> <NewValue>\n/ee emc del <ItemName>\n/ee emc export", FLinearColor::White);

	return EExecutionStatus::COMPLETED;
}

void AEEChatCommand::SendItemNotFound(UCommandSender* Sender, const FString& ItemName, TArray<TSubclassOf<UFGItemDescriptor>>& ItemDescriptors)
{
	Sender->SendChatMessage(FString::Printf(TEXT("No item found with these name.")), FLinearColor::Red);

	TArray<FString> PartiallyItemNames = SplitItemName(ItemName);
	TArray<FString> Suggestions;

	for (const TSubclassOf<UFGItemDescriptor> Descriptor : ItemDescriptors)
	{
		FText OriginalName = UFGItemDescriptor::GetItemName(Descriptor);
		TArray<FString> ItemNameParts;
		OriginalName.ToString().ToLower().ParseIntoArray(ItemNameParts, TEXT(" "));

		for (FString Part : PartiallyItemNames)
		{
			if (ItemNameParts.Contains(Part))
			{
				Suggestions.Add(OriginalName.ToString());
				break;
			}
		}
	}

	const int32 NumSuggestions = Suggestions.Num();
	if (NumSuggestions)
	{
		Sender->SendChatMessage(FString::Printf(TEXT("Do you mean one of these items?:")), FLinearColor::Red);
		for (int32 i = 0; i < NumSuggestions; ++i)
		{
			Sender->SendChatMessage(FString::Printf(TEXT("- %s"), *Suggestions[i]), FLinearColor::Red);
			if (i < 4)
			{
				continue;
			}

			if (NumSuggestions > 5)
			{
				Sender->SendChatMessage(FString::Printf(TEXT("%d more items found."), NumSuggestions - 5), FLinearColor::Red);
			}

			break;
		}
	}
}

TArray<FString> AEEChatCommand::SplitItemName(const FString& Name)
{
	TArray<FString> Result;
	int32 LastIndex = 0;
	int32 NameLen = Name.Len();

	for (int32 i = 0; i <= NameLen; ++i)
	{
		const bool bShouldSplit = i > 0 && i < NameLen && FChar::IsUpper(Name[i]);

		if (bShouldSplit || i == NameLen)
		{
			Result.Add(Name.Mid(LastIndex, i - LastIndex).ToLower());
			LastIndex = i;
		}
	}

	return Result;
}
