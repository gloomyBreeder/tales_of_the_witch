#include "ANS_AttackParams.h"

void UANS_AttackParams::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, 
	const FAnimNotifyEventReference& EventReference)
{
	AActor* owningActor = MeshComp->GetOwner();
	UCombatComponent* combatComponent = owningActor->FindComponentByClass<UCombatComponent>();

	if (!combatComponent)
	{
		return;
	}

	FVector location = MeshComp->GetSocketLocation(FName(SocketName));

	if (!ContiniousTracing)
	{
		combatComponent->GenerateHitSphere(location, Radius, DebugMode);
	}

	_slashBegin = location;
	
	//set time params
	_timeBetweenChecks = TotalDuration / static_cast<float>(NumTraceChecks);
	_checkTimer = _timeBetweenChecks;
}

void UANS_AttackParams::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	_checkTimer -= FrameDeltaTime;
	if (_checkTimer <= 0)
	{
		AActor* owningActor = MeshComp->GetOwner();
		UCombatComponent* combatComponent = owningActor->FindComponentByClass<UCombatComponent>();

		if (!combatComponent)
		{
			return;
		}
		
		FVector location = MeshComp->GetSocketLocation(FName(SocketName));
		//set end location from socket
		_slashEnd = location;

		if (ContiniousTracing)
		{
			combatComponent->GenerateHitCapsule(_slashBegin, _slashEnd, Radius, DebugMode);
		}

		//update begin location
		_slashBegin = location;
		_checkTimer = _timeBetweenChecks;
	}
}

void UANS_AttackParams::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* owningActor = MeshComp->GetOwner();
	UCombatComponent* combatComponent = owningActor->FindComponentByClass<UCombatComponent>();

	if (!combatComponent)
	{
		return;
	}

	FVector location = MeshComp->GetSocketLocation(FName(SocketName));
	_slashEnd = location;

	if (ContiniousTracing)
	{
		combatComponent->GenerateHitCapsule(_slashBegin, _slashEnd, Radius, DebugMode);
	}

	//clear set of damaged actors
	combatComponent->ClearDamageActors();
}
