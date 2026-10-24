// Copyright shenzj


#include "Character/RPGCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "RPG/RPG.h"

ARPGCharacterBase::ARPGCharacterBase()
{
 	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), "WeaponHandSocket");
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* ARPGCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARPGCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ARPGCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void ARPGCharacterBase::InitAbilityActorInfo()
{

}

void ARPGCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ARPGCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ARPGCharacterBase::AddCharacterAbilities()
{
	if (!HasAuthority()) return;
	
	URPGAbilitySystemComponent* RPGASC = CastChecked<URPGAbilitySystemComponent>(AbilitySystemComponent);
	RPGASC->AddCharacterAbilities(StartupAbilities);
}

