#pragma once

#include "CoreMinimal.h"
#include "FGCategory.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "QQuestDescriptor.generated.h"

class UTexture2D;

UCLASS(Blueprintable)
class QUESTIFY_API UQQuestDescriptor : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Quest")
	TSubclassOf<UFGCategory> Category;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Quest")
	TSubclassOf<UFGCategory> SubCategory;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Quest")
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Quest")
	FText Title;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Quest")
	FText Description;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Quest")
	float Size = 42;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Quest")
	FVector2D Position;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Quest|Requirements")
	TArray<UQQuestDescriptor*> PrerequisitesToStart;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Quest|Requirements")
	TArray<UQQuestDescriptor*> PrerequisitesToReveal;

	UFUNCTION(BlueprintCallable)
	FString ToJson();
};
