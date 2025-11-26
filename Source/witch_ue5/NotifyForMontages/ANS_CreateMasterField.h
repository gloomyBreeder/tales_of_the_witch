#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_CreateMasterField.generated.h"

UCLASS()
class WITCH_UE5_API UANS_CreateMasterField : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chaos")
    TSubclassOf<AActor> MasterFieldClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chaos")
    FName SocketName;

private:
    AActor* _masterField;
	
};
