// Copyright shenzj

#pragma once

#include "CoreMinimal.h"
#include "Character/RPGCharacterBase.h"
#include "RPGCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API ARPGCharacter : public ARPGCharacterBase
{
	GENERATED_BODY()
public:
	ARPGCharacter();
	virtual void PossessedBy(AController* NewController) override;  // Pawn 被 Controller 成功“附身”时被调用，仅服务器
	virtual void OnRep_PlayerState() override;  // 当服务器上的 PlayerState 被设置或修改并复制到客户端后，客户端引擎会调用此函数，通知 Pawn “你的 PlayerState 已经同步好了”
private:
	void InitAbilityActorInfo();
};
