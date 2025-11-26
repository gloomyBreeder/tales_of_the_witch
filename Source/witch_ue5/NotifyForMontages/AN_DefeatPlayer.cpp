#include "AN_DefeatPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "InputPlayer/InputCharacter.h"
#include "GameFramework/Character.h"

void UAN_DefeatPlayer::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* playerActor = MeshComp->GetOwner();
	if (playerActor)
	{
		ACharacter* player = UGameplayStatics::GetPlayerCharacter(playerActor->GetWorld(), 0);
		if (player)
		{
			AInputCharacter* inputPlayer = Cast<AInputCharacter>(player);
			if (inputPlayer)
			{
				inputPlayer->Die();
			}
		}
	}
}