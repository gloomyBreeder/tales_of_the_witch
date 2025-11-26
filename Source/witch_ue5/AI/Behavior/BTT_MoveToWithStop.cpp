#include "AI/Behavior/BTT_MoveToWithStop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "ActorComponents/CombatComponent.h"


UBTT_MoveToWithStop::UBTT_MoveToWithStop(FObjectInitializer const& ObjectInit)
{
    NodeName = TEXT("Move To With Stop (If Needed)");
}

EBTNodeResult::Type UBTT_MoveToWithStop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UCombatComponent* combatComp = OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<UCombatComponent>();

    if (!combatComp)
    {
        return EBTNodeResult::Failed;
    }

    if (combatComp->IsDoingAction())
    {
        AbortTask(OwnerComp, NodeMemory);
        return EBTNodeResult::Failed;
    }
    else
    {
        FVector location = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());
        OwnerComp.GetAIOwner()->MoveToLocation(location);
    }

    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTT_MoveToWithStop::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController)
    {
        AIController->StopMovement();
    }

    return EBTNodeResult::Aborted;
}
