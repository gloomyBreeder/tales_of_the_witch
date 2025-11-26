#include "BTT_FlankNearPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "ActorComponents/CombatComponent.h"
#include "AIController.h"

UBTT_FlankNearPlayer::UBTT_FlankNearPlayer(FObjectInitializer const& ObjectInit)
{
    NodeName = "Flank Near Player";
}

EBTNodeResult::Type UBTT_FlankNearPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    _cachedOwnerComp = &OwnerComp;
    AAIController* AIController = _cachedOwnerComp->GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    UObject* playerObject = _cachedOwnerComp->GetBlackboardComponent()->GetValueAsObject("AttackTarget");
    if (!playerObject)
    {
        return EBTNodeResult::Failed;
    }

    AActor* player = Cast<AActor>(playerObject);
    if (!player)
    {
        return EBTNodeResult::Failed;
    }

    FVector playerLocation = player->GetActorLocation();
    AActor* enemyPawn = AIController->GetPawn();

    if (!enemyPawn)
    {
        return EBTNodeResult::Failed;
    }

    UCombatComponent* combatComp = enemyPawn->FindComponentByClass<UCombatComponent>();

    if (!combatComp)
    {
        return EBTNodeResult::Failed;
    }

    if (combatComp->IsDoingAction())
    {
        AbortTask(OwnerComp, NodeMemory);
        return EBTNodeResult::Failed;
    }

    FVector enemyLocation = enemyPawn->GetActorLocation();
    FVector foundPoint = _cachedOwnerComp->GetBlackboardComponent()->GetValueAsVector("EnemyLocation");

    //if enemy is still moving to place
    if (!foundPoint.IsNearlyZero() && (enemyLocation - foundPoint).Size() > 0.f && (enemyPawn->GetVelocity().Length() > 0.f))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return EBTNodeResult::Succeeded;
    }

    float randomDistance = FMath::FRandRange(MinThreshold, MaxThreshold);
    // Random angle around the player (0 to 360 degrees)
    float randomAngle = FMath::RandRange(0.f, 360.f);
    FVector randomDirection = FVector(FMath::Cos(FMath::DegreesToRadians(randomAngle)), FMath::Sin(FMath::DegreesToRadians(randomAngle)), 0.f);  // Random direction in 2D
    FVector newPosition = playerLocation + randomDirection * randomDistance;

    // Validate the position on the navmesh
    UNavigationSystemV1* navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    FNavLocation projectedLocation;
    if (navSystem && navSystem->ProjectPointToNavigation(newPosition, projectedLocation))
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsChasing", true);
        AIController->MoveToLocation(projectedLocation.Location, DistanceToStopNearPlayer);
        _cachedOwnerComp->GetBlackboardComponent()->SetValueAsVector("EnemyLocation", foundPoint);
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTT_FlankNearPlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (AAIController* AIController = OwnerComp.GetAIOwner())
    {
        AIController->StopMovement();
    }

    return Super::AbortTask(OwnerComp, NodeMemory);
}
