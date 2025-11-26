#include "AI/Behavior/BTT_FindValidPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


UBTT_FindValidPoint::UBTT_FindValidPoint(FObjectInitializer const& ObjectInit)
{
    NodeName = "Find Valid Point Near The Player";
}

EBTNodeResult::Type UBTT_FindValidPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
    if (!blackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    UObject* playerObject = blackboardComp->GetValueAsObject("AttackTarget");
    if (!playerObject)
    {
        return EBTNodeResult::Failed;
    }

    AActor* player = Cast<AActor>(playerObject);
    if (!player)
    {
        return EBTNodeResult::Failed;
    }

    if (!_navSys)
    {
        _navSys = UNavigationSystemV1::GetCurrent(AIPawn->GetWorld());
    }

    FVector playerLocation = player->GetActorLocation();
    FVector actorLocation = AIPawn->GetActorLocation();
    FVector foundPoint = blackboardComp->GetValueAsVector("TargetLocation");

    //if enemy is still moving to place
    if (!foundPoint.IsNearlyZero() && (actorLocation - foundPoint).Size() > 0.f && (AIPawn->GetVelocity().Length() > 0.f))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return EBTNodeResult::Succeeded;
    }

    if (!FindIfRandomPointIsValid(playerLocation, AIPawn, foundPoint))
    {
        return EBTNodeResult::Failed;
    }

    blackboardComp->SetValueAsVector("TargetLocation", foundPoint);
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

    return EBTNodeResult::Succeeded;
}

bool UBTT_FindValidPoint::FindIfRandomPointIsValid(const FVector& PlayerLocation, APawn* AIPawn, FVector& OutPoint)
{
    if (!_navSys)
    {
        return false;
    }

    FVector randomPoint;
    for (int32 i = 0; i < 10; ++i)
    {
        _navSys->K2_GetRandomReachablePointInRadius(AIPawn->GetWorld(), PlayerLocation, randomPoint, Radius);

        if (IsPointValid(PlayerLocation, AIPawn, randomPoint) && !PathGoesTooCloseToPlayer(PlayerLocation, AIPawn, randomPoint))
        {
            OutPoint = randomPoint;
            return true;
        }
        else
        {
            continue;
        }
    }

    return false;
}

bool UBTT_FindValidPoint::PathGoesTooCloseToPlayer(const FVector& PlayerLocation, APawn* AIPawn, const FVector& Point)
{
    if (!_navSys)
    {
        return true;  // Assume failure if NavSys is unavailable
    }

    FPathFindingQuery query;
    query.StartLocation = AIPawn->GetActorLocation();
    query.EndLocation = Point;
    query.NavData = _navSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);

    FNavPathSharedPtr path = _navSys->FindPathSync(query).Path;
    if (path.IsValid() && path->IsValid())
    {
        for (const FVector& pathPoint : path->GetPathPoints())
        {
            if (FVector::DistSquared(pathPoint, PlayerLocation) <= FMath::Square(KeepOutFromPlayerRadius))
            {
                return true;  // Path goes too close to the player
            }
        }
    }

    return false;  // Path does not go near the player
}

bool UBTT_FindValidPoint::IsPointValid(const FVector& PlayerLocation, APawn* AIPawn, FVector& Point)
{
    FHitResult hit;
    APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* player = playerController->GetPawn();

    if (!player)
    {
        return false;
    }

    FVector start = Point;
    FVector directionToTarget = (PlayerLocation - Point).GetSafeNormal();
    FVector end = Point + directionToTarget * 3000.0f;

    FCollisionQueryParams params;
    params.AddIgnoredActor(AIPawn);
    bool bLineOfSight = AIPawn->GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, params);

    if (!bLineOfSight || hit.GetActor() == player)
    {
        return true;
    }
    else
    {
        return false;
    }
}
