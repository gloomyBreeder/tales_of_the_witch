#include "Actors/ElementObstacle.h"
#include "Kismet/GameplayStatics.h"
#include "Static/GameStateManager.h"

AElementObstacle::AElementObstacle()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AElementObstacle::BeginPlay()
{
	Super::BeginPlay();

	_vulnerabilityComponent = FindComponentByClass<UVulnerabilityComponent>();
	USkeletalMeshComponent* meshComponent = FindComponentByClass<USkeletalMeshComponent>();
	if (meshComponent)
	{
		_animInst = meshComponent->GetAnimInstance();
	}
	
}

void AElementObstacle::Hurt(ElementType elementType)
{
	if (_vulnerabilityComponent && _vulnerabilityComponent->GetVulnerabilityElement() != elementType)
	{
		return;
	}

	if (HurtMontage && _animInst)
	{
		_animInst->Montage_Play(HurtMontage);
	}
	
	_currentHurtNumber++;

	if (_currentHurtNumber == DestroyNumber)
	{
		UParticleSystemComponent* ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation());
		if (ParticleSystem)
		{
			ParticleSystem->SetWorldScale3D(ParticleHitScale);
		}

		// destroy actor in next tick
		_canBeDestroyed = true;
	}
}

void AElementObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_canBeDestroyed)
	{
		Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(FName("PlantHit"), ExplosionSound, GetActorLocation());
		Destroy();
	}

}

