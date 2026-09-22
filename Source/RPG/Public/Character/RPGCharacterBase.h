// Copyright shenzj

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGCharacterBase.generated.h"

UCLASS(Abstract)
class RPG_API ARPGCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ARPGCharacterBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditANywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

};
