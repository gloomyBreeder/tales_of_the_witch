#include "Dialogue/DialogueTrigger.h"
#include "Static/GameStateManager.h"

ADialogueTrigger::ADialogueTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ADialogueTrigger::EnterTrigger);
	TriggerBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ADialogueTrigger::ExitTrigger);
}

void ADialogueTrigger::BeginPlay()
{
	Super::BeginPlay();
}

void ADialogueTrigger::EnterTrigger(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent,
	int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult)
{
	if (otherActor && otherActor != this && !_isOverlapping)
	{
		AInputCharacter* playerInputCharacter = Cast<AInputCharacter>(otherActor);

		if (playerInputCharacter)
		{
			_isOverlapping = true;
			_overlappedActor = otherActor;
			ShowDialogue();
			IInteractable::Execute_OnEnterInteractionZone(otherActor, this, true, false);
		}
	}
}

void ADialogueTrigger::ExitTrigger(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, 
	int32 otherBodyIndex)
{
	if (otherActor)
	{
		_isOverlapping = false;
		_overlappedActor = nullptr;
		IInteractable::Execute_OnExitInteractionZone(otherActor, this);
	}
}

void ADialogueTrigger::InteractRequest_Implementation(AActor* interactableActor)
{
	ShowDialogue();
}

void ADialogueTrigger::OnEndDialogue(FName Dialogue)
{
	if (DialogueId == Dialogue && !_isDestroying)
	{
		_isDestroying = true;
		if (_overlappedActor)
		{
			IInteractable::Execute_OnExitInteractionZone(_overlappedActor, this);
		}
		Destroy();
	}
}

void ADialogueTrigger::ShowDialogue()
{
	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetDialogueManager()->ShowDialogue(DialogueId);
}

void ADialogueTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADialogueTrigger::Init()
{
	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetDialogueManager()->OnEndDialogue.AddUniqueDynamic(this, &ADialogueTrigger::OnEndDialogue);
}

FName ADialogueTrigger::GetDialogueId()
{
	return DialogueId;
}

