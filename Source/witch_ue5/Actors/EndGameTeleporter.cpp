#include "Actors/EndGameTeleporter.h"
#include "GameFramework/Character.h"
#include "InputPlayer/InputCharacter.h"

void AEndGameTeleporter::EnterTeleporter(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult)
{
	if (otherActor && otherActor != this && !IsTeleporting)
	{
		AGameStateManager* gameState = Cast<AGameStateManager>(GetWorld()->GetGameState());
		if (!gameState->CanEndGame())
		{
			return;
		}

		AInputCharacter* playerInputCharacter = Cast<AInputCharacter>(otherActor);
		if (playerInputCharacter)
		{
			IsTeleporting = true;
			IInteractable::Execute_OnEnterInteractionZone(otherActor, this, true, false);
			playerInputCharacter->Show(false);
			gameState->GetSoundManager()->PlaySound(EBasicSoundType::Teleport, GetActorLocation());
			gameState->GetDialogueManager()->ShowEndDialogue();
		}
	}
}

void AEndGameTeleporter::InteractRequest_Implementation(AActor* interactableActor)
{
	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetDialogueManager()->ShowEndDialogue();
}
