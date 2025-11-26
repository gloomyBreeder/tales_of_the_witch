#include "ActorComponents/CombatComponent.h"
#include "ActorComponents/VulnerabilityComponent.h"
#include "AI/Controllers/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Actors/Projectile.h"
#include "Actors/BasicLootSpawner.h"
#include "Actors/ElementObstacle.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	_animationComponent = GetOwner()->FindComponentByClass<UAnimationComponent>();
	_healthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
	_projectileSpawnComponent = GetOwner()->FindComponentByClass<UProjectileSpawnComponent>();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::TakeDamage(float damage)
{
	if (IsDying() || _isDead)
	{
		return;
	}

	//adding for difficulty - cant damage attacking enemy
	/*bool isAIControlled = Cast<AEnemyController>(Cast<ACharacter>(GetOwner())->GetController()) != nullptr;
	if (isAIControlled && (IsAttacking() || IsCasting()))
	{
		return;
	}*/

	_healthComponent->Remove(damage);

	if (_healthComponent->GetHealth() <= 0.0f)
	{
		Die();
	}
	else
	{
		Hurt();
	}
}

void UCombatComponent::Hurt()
{
	if (_animationComponent)
	{
		_animationComponent->PlayHurtAnim();
	}
}

void UCombatComponent::Restore()
{
	_isDead = false;
	_healthComponent->Restore();
}

void UCombatComponent::Die()
{
	if (_animationComponent)
	{
		_animationComponent->PlayDeathAnim();
	}

	ACharacter* character = Cast<ACharacter>(GetOwner());
	UCharacterMovementComponent* characterMovement = character->GetCharacterMovement();
	AEnemyController* enemyController = Cast<AEnemyController>(character->GetController());

	if (enemyController)
	{
		characterMovement->bOrientRotationToMovement = false;
		character->bUseControllerRotationYaw = false;
		enemyController->BrainComponent->StopLogic(TEXT("Enemy died"));

		UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(enemyController->BrainComponent);
		if (BTComponent)
		{
			BTComponent->StopTree(EBTStopMode::Forced);
		}
	}

	characterMovement->DisableMovement();
	
	if (_projectileSpawnComponent)
	{
		_projectileSpawnComponent->StopParticle();
	}

	_isDead = true;
}

bool UCombatComponent::IsAttacking()
{
	return _animationComponent && _animationComponent->IsAttacking();
}

bool UCombatComponent::IsStaggered()
{
	return _animationComponent &&  _animationComponent->IsStaggered();
}

bool UCombatComponent::IsDying()
{
	return _animationComponent && _animationComponent->IsDying();
}

bool UCombatComponent::IsCasting()
{
	return _animationComponent && _animationComponent->IsCasting();
}

void UCombatComponent::GenerateHitSphere(FVector location, float radius, bool debug)
{
	if (!GetOwner())
	{
		return;
	}
	
	TArray<FHitResult> outHits;
	TArray<AActor*> ignoredActors;
	ignoredActors.Add(GetOwner());
	FCollisionShape sphereShape;
	sphereShape.ShapeType = ECollisionShape::Sphere;
	sphereShape.SetSphere(radius);
	FCollisionQueryParams params;
	params.AddIgnoredActor(ignoredActors[0]);

	GetWorld()->SweepMultiByChannel(outHits, location, location, FQuat::Identity, ECollisionChannel::ECC_Pawn, sphereShape, params);

	//show visual traces for debugging
	if (debug)
	{
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), location, location, radius, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn),
			true, ignoredActors, EDrawDebugTrace::ForDuration, outHits, true, FColor::Red);
	}

	if (outHits.Num() > 0)
	{
		IterateHitActors(outHits);
	}
}

void UCombatComponent::GenerateHitCapsule(FVector beginLoc, FVector endLoc, float radius, bool debug)
{
	if (!GetOwner())
	{
		return;
	}
	
	TArray<FHitResult> outHits;
	TArray<AActor*> ignoredActors;
	ignoredActors.Add(GetOwner());
	FCollisionShape sphereShape;
	sphereShape.ShapeType = ECollisionShape::Sphere;
	sphereShape.SetSphere(radius);
	FCollisionQueryParams params;
	params.AddIgnoredActor(ignoredActors[0]);

	GetWorld()->SweepMultiByChannel(outHits, beginLoc, endLoc, FQuat::Identity, ECollisionChannel::ECC_Pawn, sphereShape, params);

	//show visual traces for debugging
	if (debug)
	{
		UKismetSystemLibrary::SphereTraceMulti(GetWorld(), beginLoc, endLoc, radius, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Pawn),
			true, ignoredActors, EDrawDebugTrace::ForDuration, outHits, true, FColor::Red);
	}

	if (outHits.Num() > 0)
	{
		IterateHitActors(outHits);
	}
}

void UCombatComponent::IterateHitActors(TArray<FHitResult>& outHits)
{
	bool isAIControlled  = Cast<AEnemyController>(Cast<ACharacter>(GetOwner())->GetController()) != nullptr;
	
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
			_damagedActors.Add(currentActor);
		}

		ACharacter* character = Cast<ACharacter>(currentActor);
		if (character)
		{
			//ignore enemy damaging enemy
			if (Cast<AEnemyController>(character->GetController()) && isAIControlled)
			{
				continue;
			}

			HandleDamage(character, AttackStats);

			//try adding difficulty - attack only one in row
			break;
		}
		else
		{
			AElementObstacle* obstacle = Cast<AElementObstacle>(currentActor);

			if (obstacle)
			{
				obstacle->Hurt(AttackStats.DamageType);
				break;
			}
		}
	}
}

void UCombatComponent::ClearDamageActors()
{
	_damagedActors.Empty();
}

bool UCombatComponent::IsDoingAction()
{
	return _animationComponent && _animationComponent->IsDoingAction();
}

void UCombatComponent::HandleDamage(ACharacter* victim, FAttackStats attackStats)
{
	UCombatComponent* victimCombatComp = victim->FindComponentByClass<UCombatComponent>();
	if (!victimCombatComp)
	{
		return;
	}

	if (victimCombatComp->IsDying() || victimCombatComp->IsDead())
	{
		return;
	}

	//if already staggered do nothing
	if (victimCombatComp->IsStaggered())
	{
		return;
	}

	// if enemy is victim he will see player
	if (!victim->IsPlayerControlled())
	{
		AAIController* AIController = Cast<AAIController>(victim->GetController());
		if (AIController)
		{
			UBlackboardComponent* blackBoard = AIController->GetBlackboardComponent();
			if (blackBoard)
			{
				blackBoard->SetValueAsBool("CanSeePlayer", true);
			}
		}
	}

	float damage = attackStats.Damage;

	//check vulnerability
	UVulnerabilityComponent* victimVulnerabilityComp = victim->FindComponentByClass<UVulnerabilityComponent>();
	if (victimVulnerabilityComp)
	{
		ElementType victimVulnerability = victimVulnerabilityComp->GetVulnerabilityElement();

		//get agressor damage type element
		ElementType damageType = attackStats.DamageType;

		//apply modifier for damage if both damage types are equal
		if (victimVulnerability == damageType)
		{
			damage *= victimVulnerabilityComp->GetVulnerabilityModifier();
		}
		else
		{
			damage = 0.f;
		}
	}

	if (damage > 0.f)
	{
		victimCombatComp->TakeDamage(damage);
	}
}

void UCombatComponent::Attack(FName attackName)
{
	if (_animationComponent)
	{
		_animationComponent->PlayAttackAnim(attackName);
	}
}

void UCombatComponent::Attack()
{
	if (IsStaggered())
	{
		return;
	}

	if (_animationComponent)
	{
		_animationComponent->PlayAttackAnim();
	}
}

void UCombatComponent::SetAttackStatsElement(ElementType elementType)
{
	AttackStats.DamageType = elementType;
}

bool UCombatComponent::IsDead()
{
	return _isDead;
}

void UCombatComponent::CastSpell(ElementType element, FVector endLocation)
{
	if (!_projectileSpawnComponent)
	{
		return;
	}

	bool isInAir = Cast<ACharacter>(GetOwner())->GetCharacterMovement()->IsFalling();
	bool isCasting = _animationComponent->IsCasting();

	if (!isCasting && !isInAir)
	{
		if (_animationComponent)
		{
			_animationComponent->PlayCastAnim();
		}

		if (element == ElementType::None)
		{
			element = _projectileSpawnComponent->GetDefaultElementType();
		}
		
		_projectileSpawnComponent->SpawnProjectile(element, endLocation);
	}
}
