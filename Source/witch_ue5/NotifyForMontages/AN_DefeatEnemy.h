#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_DefeatEnemy.generated.h"

UCLASS()
class WITCH_UE5_API UAN_DefeatEnemy : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
