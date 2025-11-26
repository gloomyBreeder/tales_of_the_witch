#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatStructs.h"
#include "ActorComponents/HealthComponent.h"
#include "AnimationComponent.h"
#include "ProjectileSpawnComponent.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCH_UE5_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Attack(FName attackName);
	void Attack();
	void CastSpell(ElementType element, FVector endLocation);
	void TakeDamage(float damage);
	void HandleDamage(ACharacter* victim, FAttackStats attackStats);
	void Die();
	void Hurt();
	void Restore();

	bool IsAttacking();
	bool IsStaggered();
	bool IsDying();
	bool IsCasting();
	bool IsDead();

	void GenerateHitSphere(FVector location, float radius, bool debug = false);
	void GenerateHitCapsule(FVector beginLoc, FVector endLoc, float radius, bool debug = false);
	void ClearDamageActors();
	void IterateHitActors(TArray<FHitResult>& outHits);
	void SetAttackStatsElement(ElementType elementType);
	
	//for fixing root motion and blending animations
	UFUNCTION(BlueprintCallable)
	bool IsDoingAction();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Settings")
	float Health = 150.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Settings")
	float MaxHealth = 150.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Settings")
	FAttackStats AttackStats;

private:
	TSet<AActor*> _damagedActors;
	UHealthComponent* _healthComponent;
	UAnimationComponent* _animationComponent;
	UProjectileSpawnComponent* _projectileSpawnComponent;
	int _comboAttackIndex = 0;
	bool _isDead = false;
};