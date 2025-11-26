#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Quest/QuestsData.h"
#include "Interfaces/Interactable.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "CatCharacter.generated.h"

UCLASS()
class WITCH_UE5_API ACatCharacter : public ACharacter, public IInteractable
{
	GENERATED_BODY()

public:
	ACatCharacter();
	virtual void Tick(float DeltaTime) override;
	void Init();

protected:
	virtual void BeginPlay() override;
	virtual void InteractRequest_Implementation(AActor* interactableActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* Trigger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogueCharacter CharacterName = EDialogueCharacter::Cat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Quests;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn Particles")
	UParticleSystemComponent* TeleportParticle;

	UFUNCTION()
	void Enter(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult);
	UFUNCTION()
	void Exit(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex);

private:
	//todo move it to quest component
	void StartQuest();
	void CompleteQuest();

	UFUNCTION()
	void OnEndDialogue(FName Dialogue);
	UFUNCTION()
	void OnEndQuest(FName Quest);

	int32 _currentQuestCount = -1;
	TSharedPtr<FQuestData> _currentQuest;
	AActor* _overlappedActor;
	bool _isQuestActivated = false;
	bool _isOverlapping = false;
	bool _isInDialogue = false;
	
};
