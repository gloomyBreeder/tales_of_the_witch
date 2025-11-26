#include "AI/Behavior/BTT_FindRandomLocation.h"
#include "ActorComponents/CombatComponent.h"
#include "AIController.h"
#include "AI/Controllers/EnemyController.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindRandomLocation::UBTT_FindRandomLocation(FObjectInitializer const& ObjectInit)
{
	NodeName = "Find Random Location In NavMesh";
}

EBTNodeResult::Type UBTT_FindRandomLocation::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AIController->StopMovement();
	}

	return EBTNodeResult::Aborted;
}

EBTNodeResult::Type UBTT_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* pawn = OwnerComp.GetAIOwner()->GetPawn();
	FVector actorLocation = pawn->GetActorLocation();
	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	UCombatComponent* combatComp = pawn->FindComponentByClass<UCombatComponent>();

	if (navSystem && combatComp)
	{
		if (combatComp->IsAttacking())
		{
			AbortTask(OwnerComp, NodeMemory);
			return EBTNodeResult::Failed;
		}

		ACharacter* character = Cast<ACharacter>(pawn);
		if (character)
		{
			FNavLocation location;
			if (navSystem->GetRandomPointInNavigableRadius(actorLocation, SearchRadius, location))
			{
				AEnemyController* enemyController = Cast<AEnemyController>(character->GetController());
				if (enemyController)
				{
					FVector locationToGo;
					if (enemyController->CanGoToLocation(location))
					{
						locationToGo = location.Location;
					}
					else
					{
						locationToGo = enemyController->GetHomePosition();
					}
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), locationToGo);
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

					return EBTNodeResult::Succeeded;
				}
				
			}
		}
	}

	return EBTNodeResult::Failed;
}
