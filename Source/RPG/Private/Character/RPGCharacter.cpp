// Copyright shenzj


#include "Character/RPGCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

ARPGCharacter::ARPGCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;  // 角色的移动会被限制在一个平面上
	GetCharacterMovement()->bSnapToPlaneAtStart = true;  // 游戏开始时，把角色位置吸附到这个平面上

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

}
