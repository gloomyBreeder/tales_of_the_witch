#include "BTT_ChasePlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "ActorComponents/CombatComponent.h"

UBTT_ChasePlayer::UBTT_ChasePlayer(FObjectInitializer const& ObjectInit)
{
	NodeName = TEXT("Chase Player");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    UObject* playerObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject("AttackTarget");
    AActor* player = Cast<AActor>(playerObject);
    if (!player)
    {
        return EBTNodeResult::Failed;
    }

    AActor* enemyPawn = AIController->GetPawn();
    if (!enemyPawn)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* blackboard = OwnerComp.GetBlackboardComponent();
    FVector playerLocation = player->GetActorLocation();
    FVector playerPositionAtLastCalculation = blackboard->GetValueAsVector("TargetPositionAtLastCalculation");
    FVector targetLocation = blackboard->GetValueAsVector("EnemyLocation");

    // Always start by issuing the first movement command
    AIController->MoveToLocation(targetLocation, RadiusFromPlayerToStop);
    blackboard->SetValueAsBool("IsChasing", true);

    return EBTNodeResult::InProgress;  // Task stays active for path recalculations
}

EBTNodeResult::Type UBTT_ChasePlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AIController->StopMovement();
	}

	return EBTNodeResult::Aborted;
}

void UBTT_ChasePlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* enemyPawn = AIController->GetPawn();
    UObject* playerObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject("AttackTarget");
    AActor* player = Cast<AActor>(playerObject);

    if (!enemyPawn || !player)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UCombatComponent* combatComp = enemyPawn->FindComponentByClass<UCombatComponent>();

    if (!combatComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    if (combatComp->IsDoingAction())
    {
        //AbortTask(OwnerComp, NodeMemory);
        AIController->StopMovement();
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UBlackboardComponent* blackboard = OwnerComp.GetBlackboardComponent();
    FVector playerLocation = player->GetActorLocation();
    FVector playerPositionAtLastCalculation = blackboard->GetValueAsVector("TargetPositionAtLastCalculation");
    FVector targetLocation = blackboard->GetValueAsVector("EnemyLocation");

    float distanceToLastCalculation = FVector::DistSquared(playerLocation, playerPositionAtLastCalculation);
    float distanceToTarget = FVector::Dist(enemyPawn->GetActorLocation(), targetLocation);

    // Update and move if recalculation is necessary
    if (distanceToLastCalculation > FMath::Square(RecalculateDistanceThreshold))
    {
        UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
        FNavLocation ValidatedLocation;
        if (NavSystem && NavSystem->ProjectPointToNavigation(playerLocation, ValidatedLocation))
        {
            blackboard->SetValueAsVector("EnemyLocation", ValidatedLocation.Location);
            blackboard->SetValueAsVector("TargetPositionAtLastCalculation", playerLocation);
            targetLocation = ValidatedLocation.Location;

            // Force new movement request after stopping the current one
            AIController->StopMovement();
            AIController->MoveToLocation(targetLocation, RadiusFromPlayerToStop);
        }
    }

    // Check if AI has reached the target
    if (distanceToTarget <= MaxDistanceToPlayer && enemyPawn->GetVelocity().Size() < 1.0f)
    {
        blackboard->SetValueAsBool("IsChasing", false);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
