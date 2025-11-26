#include "Projectile.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Actors/ElementObstacle.h"
#include "Components/BoxComponent.h"
#include "AI/Controllers/EnemyController.h"
#include "ActorComponents/CombatComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System Component"));
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_canMove)
	{
		return;
	}

	if (_canBeDestroyed)
	{
		OnDestroy();
	}

	if (_endLocation.Equals(FVector::ZeroVector))
	{
		return;
	}

	if (_currentDistance < _totalDistance)
	{
		FVector location = GetActorLocation();
		location += _direction * MovingSpeed * DeltaTime;
		CheckHits(location);
		SetActorLocation(location);
		_currentDistance = (location - _startLocation).Size();
	}
	else
	{
		_canMove = false;
		OnDestroy();
	}
}

void AProjectile::MoveTo(AActor* owner, FVector end, ElementType elementType)
{
	_owner = owner;
	_elementType = elementType;
	_startLocation = GetActorLocation();
	_endLocation = end;
	_direction = _endLocation - _startLocation;
	_totalDistance = _direction.Size();
	_direction = _direction.GetSafeNormal();
	CreateMasterField();
	_canMove = true;
}

void AProjectile::SetScale(const FVector scale)
{
	ParticleComponent->SetWorldScale3D(scale);
}

void AProjectile::CheckHits(FVector location)
{
	TArray<FHitResult> outHits;
	TArray<AActor*> ignoredActors;
	ignoredActors.Add(_owner);
	ignoredActors.Add(this);
	FCollisionShape lineShape;
	lineShape.ShapeType = ECollisionShape::Line;
	FCollisionQueryParams params;
	params.AddIgnoredActor(_owner);
	params.AddIgnoredActor(this);
	FVector currentLocation = GetActorLocation();

	GetWorld()->SweepMultiByChannel(outHits, currentLocation, location, FQuat::Identity, ECollisionChannel::ECC_Pawn, lineShape, params);

	//debug
	/*UKismetSystemLibrary::LineTraceMulti(GetWorld(), currentLocation, _endLocation, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn),
		true, ignoredActors, EDrawDebugTrace::ForDuration, outHits, true, FColor::Red);*/

	bool isOwnerAIControlled = Cast<AEnemyController>(Cast<ACharacter>(_owner)->GetController()) != nullptr;
	
	if (outHits.Num() > 0)
	{
		for (auto i = outHits.CreateIterator(); i; i++)
		{
			AActor* currentActor = i->GetActor();
			if (!currentActor)
			{
				continue;
			}

			if (_damagedActors.Contains(currentActor))
			{
				continue;
			}
			else
			{
				if (IsEmptyOrTriggerOnly(currentActor))
				{
					continue;
				}

				_damagedActors.Add(currentActor);
			}

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, currentActor->GetName());

			ACharacter* victim = Cast<ACharacter>(currentActor);
			if (victim)
			{
				//ignore enemy damaging enemy
				if (Cast<AEnemyController>(victim->GetController()) && isOwnerAIControlled)
				{
					continue;
				}

				UCombatComponent* combatComponent = victim->FindComponentByClass<UCombatComponent>();
				if (combatComponent)
				{
					combatComponent->HandleDamage(victim, AttackStats);
					PlayParticleHit();
					//try adding difficulty - attack only one in row
					break;
				}
			}
			else
			{
				AElementObstacle* obstacle = Cast<AElementObstacle>(currentActor);

				if (obstacle)
				{
					obstacle->Hurt(_elementType);
					PlayParticleHit();
					break;
				}

				//ignore already broken collections because there is no way to check collisions per level (fy geometry collection with chaos)
				/*UGeometryCollectionComponent* Collection = currentActor->FindComponentByClass<UGeometryCollectionComponent>();
				if (Collection)
				{
					if (Collection->IsRootBroken())
					{
						continue;
					}
				}*/
			}

			PlayParticleHit();
		}
	}
}

bool AProjectile::IsEmptyOrTriggerOnly(AActor* actor)
{
	TArray<UPrimitiveComponent*> components;
	actor->GetComponents<UPrimitiveComponent>(components);

	for (UActorComponent* comp : components)
	{
		UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(comp);
		if (!primComp)
		{
			// ignore other components
			continue; 
		}

		//if no trigger box but it is primitive it has mesh or smth
		if (!primComp->IsA<UBoxComponent>())
		{
			return false;
		}
	}

	return true;
}

void AProjectile::CreateMasterField()
{
	_masterField = GetWorld()->SpawnActor<AActor>(MasterFieldClass, FVector::ZeroVector, FRotator::ZeroRotator);

	if (_masterField)
	{
		_masterField->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void AProjectile::PlayParticleHit()
{
	ParticleComponent->Complete();
	UParticleSystemComponent* ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation());
	if (ParticleSystem)
	{
		Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EBasicSoundType::HitProjectile);
		ParticleSystem->SetWorldScale3D(ParticleHitScale);
	}

	// destroy actor in next tick
	_canBeDestroyed = true;
}

void AProjectile::OnDestroy()
{
	if (_masterField)
	{
		_masterField->Destroy();
		_masterField = nullptr;
	}

	this->K2_DestroyActor();
}
