#include "AI/Behavior/BTS_IsNeedToStop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "ActorComponents/CombatComponent.h"

UBTS_IsNeedToStop::UBTS_IsNeedToStop()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Need To Stop");
}

void UBTS_IsNeedToStop::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	APawn* pawn = OwnerComp.GetAIOwner()->GetPawn();
	UCombatComponent* combatComp = pawn->FindComponentByClass<UCombatComponent>();

	if (!combatComp)
	{
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	if (combatComp->IsDoingAction())
	{
		AIController->StopMovement();
	}
}
