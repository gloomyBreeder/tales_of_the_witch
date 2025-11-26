#include "BTS_IsPlayerInRange.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTS_IsPlayerInRange::UBTS_IsPlayerInRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Player In Range");
}

void UBTS_IsPlayerInRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	APawn* controlledPawn = AIController->GetPawn();
	if (!controlledPawn)
	{
		return;
	}

	ACharacter* character = Cast<ACharacter>(controlledPawn);
	if (!character)
	{
		return;
	}

	UCharacterMovementComponent* movementComponent = character->GetCharacterMovement();

	if (!_isCached)
	{
		_isCached = true;
		_useControllerDesiredRotation = movementComponent->bUseControllerDesiredRotation;
		_orientRotationToMovement = movementComponent->bOrientRotationToMovement;
		_useControllerRotationYaw = controlledPawn->bUseControllerRotationYaw;
	}

	bool isInRange = controlledPawn->GetDistanceTo(Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))) <= Range;

	if (isInRange)
	{
		movementComponent->bUseControllerDesiredRotation = true;
		movementComponent->bOrientRotationToMovement = false;
		controlledPawn->bUseControllerRotationYaw = true;
	}
	else
	{
		movementComponent->bUseControllerDesiredRotation = _useControllerDesiredRotation;
		movementComponent->bOrientRotationToMovement = _orientRotationToMovement;
		controlledPawn->bUseControllerRotationYaw = _useControllerRotationYaw;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), isInRange);
}
