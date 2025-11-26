#include "AnimationComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimationComponent::UAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateAnimInst();
}

void UAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAnimationComponent::PlayHealAnim()
{
	if (!_animInst)
	{
		return;
	}

	if (HealMontage)
	{
		_animInst->Montage_Play(HealMontage);
	}
}

void UAnimationComponent::PlayHurtAnim()
{
	if (!_animInst)
	{
		return;
	}

	if (HurtMontage)
	{
		_animInst->Montage_Play(HurtMontage);
	}
}

void UAnimationComponent::PlayDeathAnim()
{
	if (!_animInst)
	{
		return;
	}

	if (DeathMontage)
	{
		_animInst->Montage_Play(DeathMontage);
	}
}

bool UAnimationComponent::IsAttacking()
{
	if (AttackMap.Num() == 0)
	{
		return false;
	}

	if (!_animInst)
	{
		return false;
	}

	// check map keys to find out if any of them currently playing
	TArray<FName> mapKeys;
	AttackMap.GetKeys(mapKeys);

	for (int i = 0; i < mapKeys.Num(); i++)
	{
		if (_animInst->Montage_IsPlaying(AttackMap.Find(mapKeys[i])->AttackMontage))
		{
			return true;
		}
	}

	return false;
}

bool UAnimationComponent::IsStaggered()
{
	if (!_animInst)
	{
		return false;
	}

	return _animInst->Montage_IsPlaying(HurtMontage);
}

bool UAnimationComponent::IsDying()
{
	if (!_animInst)
	{
		return false;
	}

	return _animInst->Montage_IsPlaying(DeathMontage);
}

bool UAnimationComponent::IsCasting()
{
	if (!_animInst)
	{
		return false;
	}

	return _animInst->Montage_IsPlaying(CastMontage);
}

bool UAnimationComponent::IsHealing()
{
	if (!_animInst)
	{
		return false;
	}

	return _animInst->Montage_IsPlaying(HealMontage);
}

void UAnimationComponent::PlayAttackAnim(FName attackName)
{
	if (AttackMap.Num() == 0)
	{
		return;
	}

	if (!_animInst)
	{
		return;
	}

	FAttack* attack = AttackMap.Find(attackName);

	if (!attack)
	{
		return;
	}

	UAnimMontage* attackMontage = attack->AttackMontage;

	if (attackMontage)
	{
		_animInst->Montage_Play(attackMontage);
	}
}

void UAnimationComponent::PlayAttackAnim()
{
	if (!_animInst)
	{
		return;
	}

	if (!LightAttackMontage)
	{
		return;
	}

	ACharacter* character = Cast<ACharacter>(GetOwner());
	UCharacterMovementComponent* characterMovement = character->GetCharacterMovement();
	bool isInAir = characterMovement->IsFalling();

	if (!IsAttacking() && !isInAir && !IsHealing() && !IsStaggered())
	{
		if (LightAttackMontage)
		{
			_animInst->Montage_Play(LightAttackMontage);
		}
	}
	else
	{
		_comboAttackIndex = 1;
	}
}

void UAnimationComponent::HandleOnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPayload)
{
	_comboAttackIndex--;
	if (notifyName.ToString() == "OnComboAnimChange")
	{
		//stop montage if below 0
		if (_comboAttackIndex < 0)
		{
			if (_animInst)
			{
				_animInst->Montage_Stop(0.25f, LightAttackMontage);
			}
		}
	}
	else if (notifyName.ToString() == "OnComboAnimEnd")
	{
		_comboAttackIndex = 0;
	}
}

void UAnimationComponent::PlayCastAnim()
{
	if (!_animInst)
	{
		return;
	}

	if (CastMontage)
	{
		_animInst->Montage_Play(CastMontage);
	}
}

bool UAnimationComponent::IsDoingAction()
{
	return IsAttacking() || IsCasting() || IsStaggered() || IsHealing();
}

void UAnimationComponent::UpdateAnimInst()
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (character)
	{
		USkeletalMeshComponent* mesh = character->GetMesh();
		if (mesh)
		{
			_animInst = mesh->GetAnimInstance();
		}
	}

	if (!_animInst)
	{
		return;
	}

	//need it only for player for now
	bool isPlayer = character->IsPlayerControlled();
	if (_animInst && isPlayer)
	{
		if (isPlayer)
		{
			_animInst->OnPlayMontageNotifyBegin.AddDynamic(this, &UAnimationComponent::HandleOnMontageNotifyBegin);
		}
	}
}

