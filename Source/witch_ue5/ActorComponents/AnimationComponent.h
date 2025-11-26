#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatStructs.h"
#include "AnimationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCH_UE5_API UAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAnimationComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PlayHealAnim();
	void PlayHurtAnim();
	void PlayDeathAnim();
	bool IsAttacking();
	bool IsStaggered();
	bool IsDying();
	bool IsCasting();
	bool IsHealing();
	void PlayAttackAnim(FName attackName);
	void PlayAttackAnim();
	void PlayCastAnim();
	bool IsDoingAction();
	void UpdateAnimInst();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPayload);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HealMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HurtMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* LightAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CastMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FAttack> AttackMap;

private:
	UAnimInstance* _animInst;
	int _comboAttackIndex = 0;
};
