#pragma once

#include "CoreMinimal.h"
#include "Actors/MagicTeleporter.h"
#include "Interfaces/Interactable.h"
#include "EndGameTeleporter.generated.h"

UCLASS()
class WITCH_UE5_API AEndGameTeleporter : public AMagicTeleporter, public IInteractable
{
	GENERATED_BODY()

protected:
	virtual void EnterTeleporter(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult) override;
	virtual void InteractRequest_Implementation(AActor* interactableActor) override;
};
