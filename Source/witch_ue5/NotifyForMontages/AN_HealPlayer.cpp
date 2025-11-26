#include "AN_HealPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "ActorComponents/HealComponent.h"

void UAN_HealPlayer::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* playerActor = MeshComp->GetOwner();
	if (playerActor)
	{
		ACharacter* player = UGameplayStatics::GetPlayerCharacter(playerActor->GetWorld(), 0);
		if (player)
		{
			UHealComponent* healComponent = player->FindComponentByClass<UHealComponent>();
			if (healComponent)
			{
				healComponent->Heal();
			}
		}
	}
}
