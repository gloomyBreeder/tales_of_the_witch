#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Interfaces/Interactable.h"
#include "DialogueTrigger.generated.h"

UCLASS()
class WITCH_UE5_API ADialogueTrigger : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ADialogueTrigger();
	virtual void Tick(float DeltaTime) override;
	void Init();

	FName GetDialogueId();

protected:
	virtual void BeginPlay() override;
	virtual void InteractRequest_Implementation(AActor* interactableActor) override;

	UFUNCTION()
	virtual void EnterTrigger(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult);
	UFUNCTION()
	virtual void ExitTrigger(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex);
	UFUNCTION()
	void OnEndDialogue(FName Dialogue);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* TriggerBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DialogueId;

private:
	bool _isOverlapping = false;
	bool _isDestroying = false;
	AActor* _overlappedActor = nullptr;

	void ShowDialogue();
};
