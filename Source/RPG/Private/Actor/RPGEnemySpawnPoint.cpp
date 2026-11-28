// Copyright shenzj


#include "Actor/RPGEnemySpawnPoint.h"

#include "Character/RPGEnemy.h"

void ARPGEnemySpawnPoint::SpawnEnemy()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	ARPGEnemy* Enemy = GetWorld()->SpawnActorDeferred<ARPGEnemy>(EnemyClass, GetActorTransform());
	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
