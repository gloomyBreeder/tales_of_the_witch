#include "NotifyForMontages/ANS_CreateMasterField.h"

void UANS_CreateMasterField::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    _masterField = MeshComp->GetWorld()->SpawnActor<AActor>(MasterFieldClass, MeshComp->GetSocketLocation(SocketName), FRotator::ZeroRotator);

    if (_masterField)
    {
        _masterField->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
    }
}

void UANS_CreateMasterField::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (_masterField)
    {
        _masterField->Destroy();
        _masterField = nullptr;
    }
}
