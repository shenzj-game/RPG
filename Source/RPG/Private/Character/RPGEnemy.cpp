// Copyright shenzj


#include "Character/RPGEnemy.h"

#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGAbilitySystemLibrary.h"
#include "AbilitySystem/RPGAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "RPG/RPG.h"
#include "UI/Widget/RPGUserWidget.h"


ARPGEnemy::ARPGEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	AbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<URPGAttributeSet>("AttributeSet");
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void ARPGEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void ARPGEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 ARPGEnemy::GetPlayerLevel()
{
	return Level;
}

void ARPGEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();

	if (URPGUserWidget* RPGUserWidget = Cast<URPGUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		RPGUserWidget->SetWidgetController(this);
	}
	
	if (const URPGAttributeSet* RPGAS = Cast<URPGAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RPGAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(RPGAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		OnHealthChanged.Broadcast(RPGAS->GetHealth());
		OnMaxHealthChanged.Broadcast(RPGAS->GetMaxHealth());
	}
	
}

void ARPGEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<URPGAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	
	InitializeDefaultAttributes();
}

void ARPGEnemy::InitializeDefaultAttributes() const
{
	URPGAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
	
}
