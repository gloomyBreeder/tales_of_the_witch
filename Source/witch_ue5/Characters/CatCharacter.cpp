#include "Characters/CatCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Static/GameStateManager.h"
#include "Quest/QuestManager.h"
#include "InputPlayer/InputCharacter.h"

ACatCharacter::ACatCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	Trigger->SetupAttachment(RootComponent);
}

void ACatCharacter::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddUniqueDynamic(this, &ACatCharacter::Enter);
	Trigger->OnComponentEndOverlap.AddUniqueDynamic(this, &ACatCharacter::Exit);
}

void ACatCharacter::Enter(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, 
	int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult)
{
	if (otherActor && otherActor != this && !_isOverlapping)
	{
		bool hasInterface = UKismetSystemLibrary::DoesImplementInterface(otherActor, UInteractable::StaticClass());

		if (hasInterface)
		{
			_isOverlapping = true;

			if (_isQuestActivated)
			{
				return;
			}

			_overlappedActor = otherActor;

			// no quests => return
			if (_currentQuestCount + 1 >= Quests.Num())
			{
				return;
			}

			FRotator newRotation = (otherActor->GetActorLocation() - GetActorLocation()).Rotation();
			SetActorRotation(newRotation);
			IInteractable::Execute_OnEnterInteractionZone(otherActor, this, true, true);
			_isQuestActivated = true;
			StartQuest();
		}
	}
}

void ACatCharacter::Exit(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, 
	int32 otherBodyIndex)
{
	if (otherActor && otherActor != this && _isOverlapping && !_isInDialogue)
	{
		bool hasInterface = UKismetSystemLibrary::DoesImplementInterface(otherActor, UInteractable::StaticClass());

		if (hasInterface)
		{
			_isOverlapping = false;
			IInteractable::Execute_OnExitInteractionZone(otherActor, this);
			_overlappedActor = nullptr;
		}
	}
}

void ACatCharacter::InteractRequest_Implementation(AActor* interactableActor)
{
	if (_currentQuest)
	{
		Cast<AGameStateManager>(GetWorld()->GetGameState())->GetQuestManager()->ShowQuestDialogue(_currentQuest->QuestId);
	}
}

void ACatCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACatCharacter::Init()
{
	AGameStateManager* gameState = Cast<AGameStateManager>(GetWorld()->GetGameState());
	UQuestManager* questManager = gameState->GetQuestManager();
	questManager->OnQuestEnd.AddUniqueDynamic(this, &ACatCharacter::OnEndQuest);
	gameState->GetDialogueManager()->OnEndDialogue.AddUniqueDynamic(this, &ACatCharacter::OnEndDialogue);
}

void ACatCharacter::StartQuest()
{
	_currentQuestCount++;
	_isInDialogue = true;

	FName currentQuestName = Quests[_currentQuestCount];
	_currentQuest = Cast<AGameStateManager>(GetWorld()->GetGameState())->GetQuestManager()->GetQuestData(currentQuestName);
	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetQuestManager()->StartQuest(currentQuestName);
}

void ACatCharacter::CompleteQuest()
{
	_isQuestActivated = false;
	_currentQuest = nullptr;
	
	if (_currentQuestCount == Quests.Num() - 1)
	{
		Cast<AGameStateManager>(GetWorld()->GetGameState())->SetCanEndGame();
		Destroy();
	}
}

void ACatCharacter::OnEndQuest(FName Quest)
{
	if (_currentQuest && _currentQuest->QuestId == Quest)
	{
		CompleteQuest();
	}
}

void ACatCharacter::OnEndDialogue(FName Dialogue)
{
	if (_isInDialogue && _currentQuest && _currentQuest->DialogueId == Dialogue)
	{
		_isInDialogue = false;
		_isOverlapping = false;
		if (_overlappedActor)
		{
			IInteractable::Execute_OnExitInteractionZone(_overlappedActor, this);
			_overlappedActor = nullptr;
		}

		Cast<AGameStateManager>(GetWorld()->GetGameState())->GetQuestManager()->TryCompleteQuest(_currentQuest->QuestId);
	}
	
}


