// Copyright shenzj

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/TargetPoint.h"
#include "RPGEnemySpawnPoint.generated.h"

class ARPGEnemy;
/**
 * 
 */
UCLASS()
class RPG_API ARPGEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Class")
	TSubclassOf<ARPGEnemy> EnemyClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Class")
	int32 EnemyLevel = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Class")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
};
