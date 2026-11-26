// Copyright shenzj

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RPGGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API URPGGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName PlayerStartTag = FName();
	
	UPROPERTY()
	FString LoadSlotName = FString();
	
	UPROPERTY()
	int32 LoadSlotIndex = 0;
	
	
};
