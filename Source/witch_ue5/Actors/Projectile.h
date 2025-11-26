#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Combat/CombatStructs.h"
#include "Projectile.generated.h"

UCLASS()
class WITCH_UE5_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;

	void MoveTo(AActor* owner, FVector end, ElementType elementType);
	void SetScale(const FVector scale);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile settings")
	float MovingSpeed = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile settings")
	FAttackStats AttackStats;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile settings")
	UParticleSystemComponent* ParticleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile settings")
	UParticleSystem* HitParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile settings")
	FVector ParticleHitScale = FVector(0.15f, 0.15f, 0.15f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile settings")
	TSubclassOf<AActor> MasterFieldClass;
	
private:
	void CheckHits(FVector location);
	void CreateMasterField();
	void PlayParticleHit();
	void OnDestroy();
	bool IsEmptyOrTriggerOnly(AActor* actor);

	bool _canMove = false;
	FVector _endLocation = FVector::ZeroVector;
	FVector _startLocation;
	FVector _direction;
	float _totalDistance;
	float _currentDistance = 0.f;
	AActor* _owner;
	TSet<AActor*> _damagedActors;
	AActor* _masterField;
	ElementType _elementType;
	bool _canBeDestroyed = false;

};
