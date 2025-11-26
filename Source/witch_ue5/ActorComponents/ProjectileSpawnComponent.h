#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/Projectile.h"
#include "ProjectileSpawnComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCH_UE5_API UProjectileSpawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UProjectileSpawnComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SpawnProjectile(ElementType element, FVector endLocation);
	void Init(ElementType element);
	void StopParticle();
	void UpdateAnimInst();
	ElementType GetDefaultElementType();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Projectile Particle")
	ElementType DefaultElementType = ElementType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Projectile Particle")
	FName SpawnSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Projectile Particle")
	FVector ParticleScale = FVector(0.15f, 0.15f, 0.15f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Projectile Particle")
	float DelayAfterShoot = 1.f;

	UFUNCTION()
	void HandleOnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPayload);

private:
	void GetData();
	void SpawnProjectile();
	void OnTimerEnd();

	FVector _endLocation;
	ElementType _element;
	FTransform _socketLocation;
	AProjectile* _projectile;
	USkeletalMeshComponent* _parentMesh;
	FTimerHandle _timerHandle;

	UPROPERTY()
	UDataTable* _dataTable;
};
