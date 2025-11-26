#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_HealPlayer.generated.h"

UCLASS()
class WITCH_UE5_API UAN_HealPlayer : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
