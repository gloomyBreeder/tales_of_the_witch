#include "AI/Behavior/BTS_IsInValidPointNearThePlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "ActorComponents/CombatComponent.h"

UBTS_IsInValidPointNearThePlayer::UBTS_IsInValidPointNearThePlayer()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is In Valid Point Near The Player");
}

void UBTS_IsInValidPointNearThePlayer::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return;
    }

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
    {
        return;
    }

    UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
    if (!blackboardComp)
    {
        return;
    }

    UObject* playerObject = blackboardComp->GetValueAsObject("AttackTarget");
    if (!playerObject)
    {
        return;
    }

    AActor* player = Cast<AActor>(playerObject);
    if (!player)
    {
        return;
    }

    FVector playerLocation = player->GetActorLocation();
    FVector actorLocation = AIPawn->GetActorLocation();
    FVector foundPoint;

    if (IsPointValid(playerLocation, AIPawn, actorLocation) && AIPawn->GetDistanceTo(player) > MinimumDistance)
    {
        foundPoint = actorLocation;
        blackboardComp->SetValueAsVector("TargetLocation", foundPoint);
        blackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), true);
    }
    else
    {
        blackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), false);
    }
}

bool UBTS_IsInValidPointNearThePlayer::IsPointValid(const FVector& PlayerLocation, APawn* AIPawn, FVector& Point)
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

    //for debug
    //DrawDebugLine(GetWorld(), Start, End, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 1.0f, 0, 5.0f); 

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
