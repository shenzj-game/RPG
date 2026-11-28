// Copyright shenzj


#include "Checkpoint/MapEntrance.h"

#include "Components/SphereComponent.h"
#include "Game/RPGGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AMapEntrance::LoadActor_Implementation()
{

}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		
		if (ARPGGameModeBase* RPGGameMode = Cast<ARPGGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			RPGGameMode->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}
		
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);
		
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}
