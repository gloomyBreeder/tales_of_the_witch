#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_DefeatPlayer.generated.h"

UCLASS()
class WITCH_UE5_API UAN_DefeatPlayer : public UAnimNotify
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
