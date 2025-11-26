#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "ActorComponents/VulnerabilityComponent.h"
#include "ElementObstacle.generated.h"

UCLASS()
class WITCH_UE5_API AElementObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	AElementObstacle();
	virtual void Tick(float DeltaTime) override;

	void Hurt(ElementType elementType);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	int32 DestroyNumber = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	UAnimMontage* HurtMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	UParticleSystem* HitParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	FVector ParticleHitScale = FVector(0.15f, 0.15f, 0.15f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	USoundBase* ExplosionSound;

private:
	UVulnerabilityComponent* _vulnerabilityComponent;
	UAnimInstance* _animInst;

	bool _canBeDestroyed = false;
	int32 _currentHurtNumber = 0;

};
