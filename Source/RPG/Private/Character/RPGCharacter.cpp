// Copyright shenzj


#include "Character/RPGCharacter.h"

#include "NiagaraComponent.h"
#include "RPGGameplayTags.h"
#include "Player/RPGPlayerState.h"
#include "Player/RPGPlayerController.h"
#include "UI/HUD/RPGHUD.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGAbilitySystemLibrary.h"
#include "AbilitySystem/RPGAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Game/LoadScreenSaveGame.h"
#include "Game/RPGGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

ARPGCharacter::ARPGCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;  // 角色的移动会被限制在一个平面上
	GetCharacterMovement()->bSnapToPlaneAtStart = true;  // 游戏开始时，把角色位置吸附到这个平面上

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CharacterClass = ECharacterClass::Elementalist;
}

void ARPGCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitAbilityActorInfo();
	LoadProgress();
	
	if (ARPGGameModeBase* RPGGameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		RPGGameMode->LoadWorldState(GetWorld());
	}
}

void ARPGCharacter::LoadProgress()
{
	ARPGGameModeBase* RPGGameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (RPGGameMode)
	{
		ULoadScreenSaveGame* SaveData = RPGGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;
		
		if (SaveData->bFirstTimeLoadIn)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}
		else
		{
			if (URPGAbilitySystemComponent* RPGASC = Cast<URPGAbilitySystemComponent>(AbilitySystemComponent))
			{
				RPGASC->AddCharacterAbilitiesFromSaveData(SaveData);
			}
		
			if (ARPGPlayerState* RPGPlayerState = Cast<ARPGPlayerState>(GetPlayerState()))
			{
				RPGPlayerState->SetLevel(SaveData->PlayerLevel);
				RPGPlayerState->SetXP(SaveData->XP);
				RPGPlayerState->SetAttributePoints(SaveData->AttributePoints);
				RPGPlayerState->SetSpellPoints(SaveData->SpellPoints);
			}
			URPGAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveData);
		}
	}
}

void ARPGCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

int32 ARPGCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	return RPGPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 ARPGCharacter::GetXP_Implementation() const
{
	const ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	return RPGPlayerState->GetXP();
}

int32 ARPGCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	const ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	return RPGPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 ARPGCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	const ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	return RPGPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

int32 ARPGCharacter::GetAttributePoints_Implementation() const
{
	const ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	return RPGPlayerState->GetAttributePoints();
}

int32 ARPGCharacter::GetSpellPoints_Implementation() const
{
	const ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	return RPGPlayerState->GetSpellPoints();
}

void ARPGCharacter::AddToXP_Implementation(int32 InXP)
{
	ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	RPGPlayerState->AddToXP(InXP);
}

void ARPGCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	RPGPlayerState->AddToLevel(InPlayerLevel);
	
	if (URPGAbilitySystemComponent* RPGASC = Cast<URPGAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		RPGASC->UpdateAbilityStatuses(RPGPlayerState->GetPlayerLevel());
	}
}

void ARPGCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	RPGPlayerState->AddToAttributePoints(InAttributePoints);
}

void ARPGCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	RPGPlayerState->AddToSpellPoints(InSpellPoints);
}

void ARPGCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

void ARPGCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (ARPGPlayerController* RPGPlayerController = Cast<ARPGPlayerController>(GetController()))
	{
		RPGPlayerController->ShowMagicCircle(DecalMaterial);
		RPGPlayerController->bShowMouseCursor = false;
	}
}

void ARPGCharacter::HideMagicCircle_Implementation()
{
	if (ARPGPlayerController* RPGPlayerController = Cast<ARPGPlayerController>(GetController()))
	{
		RPGPlayerController->HideMagicCircle();
		RPGPlayerController->bShowMouseCursor = true;
	}
}

void ARPGCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	ARPGGameModeBase* RPGGameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (RPGGameMode)
	{
		ULoadScreenSaveGame* SaveData = RPGGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;
		
		SaveData->PlayerStartTag = CheckpointTag;
		
		if (ARPGPlayerState* RPGPlayerState = Cast<ARPGPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = RPGPlayerState->GetPlayerLevel();
			SaveData->XP = RPGPlayerState->GetXP();
			SaveData->AttributePoints = RPGPlayerState->GetAttributePoints();
			SaveData->SpellPoints = RPGPlayerState->GetSpellPoints();
		}
		SaveData->Strength = URPGAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = URPGAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Resilience = URPGAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Vigor = URPGAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
		
		SaveData->bFirstTimeLoadIn = false;
		
		if (!HasAuthority()) return;
		
		URPGAbilitySystemComponent* RPGASC = Cast<URPGAbilitySystemComponent>(AbilitySystemComponent);
		FForEachAbility SaveAbilityDelegate;
		SaveData->SavedAbilities.Empty();
		SaveAbilityDelegate.BindLambda(
			[this, RPGASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
			{
				const FGameplayTag AbilityTag = RPGASC->GetAbilityTagFromSpec(AbilitySpec);
				UAbilityInfo* AbilityInfo = URPGAbilitySystemLibrary::GetAbilityInfo(this);
				FRPGAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
				
				FSavedAbility SavedAbility;
				SavedAbility.GameplayAbility = Info.Ability;
				SavedAbility.AbilityLevel = AbilitySpec.Level;
				SavedAbility.AbilitySlot = RPGASC->GetSlotFromAbilityTag(AbilityTag);
				SavedAbility.AbilityStatus = RPGASC->GetStatusFromAbilityTag(AbilityTag);
				SavedAbility.AbilityTag = AbilityTag;
				SavedAbility.AbilityType = Info.AbilityType;
				
				SaveData->SavedAbilities.AddUnique(SavedAbility);
			}
		);
		RPGASC->ForEachAbility(SaveAbilityDelegate);
		
		RPGGameMode->SaveInGameProgressData(SaveData);
	}
}

void ARPGCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}


int32 ARPGCharacter::GetPlayerLevel_Implementation()
{
	const ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	return RPGPlayerState->GetPlayerLevel();
}

void ARPGCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);
	
	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda(
		[this]()
		{
			ARPGGameModeBase* RPGGameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this));
			if (RPGGameMode)
			{
				RPGGameMode->PlayerDied(this);
			}
		}
	);
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);
	TopDownCameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void ARPGCharacter::OnRep_Stunned()
{
	if (URPGAbilitySystemComponent* RPGASC = Cast<URPGAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FRPGGameplayTags& GameplayTags = FRPGGameplayTags::Get();
		
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
		if (bIsStunned)
		{
			RPGASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		}
		else
		{
			RPGASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void ARPGCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void ARPGCharacter::InitAbilityActorInfo()
{
	ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>();
	check(RPGPlayerState);
	RPGPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(RPGPlayerState, this);
	Cast<URPGAbilitySystemComponent>(RPGPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = RPGPlayerState->GetAbilitySystemComponent();
	AttributeSet = RPGPlayerState->GetAttributeSet();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FRPGGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ARPGCharacter::StunTagChanged);

	if (ARPGPlayerController* RPGPlayerController = Cast<ARPGPlayerController>(GetController()))
	{
		if (ARPGHUD* RPGHUD = Cast<ARPGHUD>(RPGPlayerController->GetHUD()))
		{
			RPGHUD->InitOverlay(RPGPlayerController, RPGPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}
