#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Combat/CombatStructs.h"
#include "ActorComponents/CombatComponent.h"
#include "ANS_AttackParams.generated.h"

UCLASS()
class WITCH_UE5_API UANS_AttackParams : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	//socket used for hurt box generation
	UPROPERTY(EditAnywhere)
	FString SocketName;

	// radius of hit sphere or capsule
	UPROPERTY(EditAnywhere)
	float Radius;
	
	//continious slash tracking or single check
	UPROPERTY(EditAnywhere)
	bool ContiniousTracing;

	//number of traces to check along slash path
	UPROPERTY(EditAnywhere)
	int NumTraceChecks;

	//debug mode for visual slash path
	UPROPERTY(EditAnywhere)
	bool DebugMode;

	UFUNCTION()
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, 
		const FAnimNotifyEventReference& EventReference) override;

	UFUNCTION()
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
		const FAnimNotifyEventReference& EventReference) override;

	UFUNCTION()
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	//location tracking
	FVector _slashBegin;
	FVector _slashEnd;

	//tracking timing of checks
	float _timeBetweenChecks = 0.005f;
	float _checkTimer = 0.005f;
};
