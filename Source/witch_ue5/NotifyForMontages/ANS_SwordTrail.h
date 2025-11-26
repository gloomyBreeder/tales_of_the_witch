#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState_Trail.h"
#include "ANS_SwordTrail.generated.h"

UCLASS()
class WITCH_UE5_API UANS_SwordTrail : public UAnimNotifyState_Trail
{
	GENERATED_BODY()

	virtual UParticleSystem* GetOverridenPSTemplate(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomTrails")
	UParticleSystem* FireTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomTrails")
	UParticleSystem* WaterTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomTrails")
	UParticleSystem* LightningTrail;
};
