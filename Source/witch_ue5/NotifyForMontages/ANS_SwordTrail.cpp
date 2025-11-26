#include "NotifyForMontages/ANS_SwordTrail.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "InputPlayer/InputCharacter.h"
#include "ANS_SwordTrail.h"

UParticleSystem* UANS_SwordTrail::GetOverridenPSTemplate(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	AActor* owningActor = MeshComp->GetOwner();
	AInputCharacter* character = Cast<AInputCharacter>(owningActor);

	if (character)
	{
		ElementType currentElement = character->GetCurrentElement();

		switch (currentElement)
		{
			case ElementType::Fire:
				return FireTrail;
			case ElementType::Water:
				return WaterTrail;
			case ElementType::Lightning:
				return LightningTrail;
			default:
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "No such element type");
				break;
		}
	}

	return nullptr;
}
