#include "BTT_Attack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Controllers/EnemyController.h"
#include "ActorComponents/CombatComponent.h"

UBTT_Attack::UBTT_Attack(FObjectInitializer const& ObjectInit)
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* pawn = OwnerComp.GetAIOwner()->GetPawn();
	UCombatComponent* combatComp = pawn->FindComponentByClass<UCombatComponent>();

	if (!combatComp)
	{
		return EBTNodeResult::Failed;
	}

	if (!combatComp->IsDoingAction() && !combatComp->IsDead())
	{
		OwnerComp.GetAIOwner()->StopMovement();
		AEnemyController* enemyController = Cast<AEnemyController>(pawn->GetController());
		CombatEnemyType enemyType = enemyController->CombatEnemyType;
		bool isNobodyAttacking = enemyController->CanAttack();

		if (isNobodyAttacking)
		{
			if (enemyType == CombatEnemyType::Melee)
			{
				bool isChasing = OwnerComp.GetBlackboardComponent()->GetValueAsBool("IsChasing");
				if (isChasing)
				{
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
				combatComp->Attack(Attack);
			}
			else if (enemyType == CombatEnemyType::Magic)
			{
				//if out of range => dont attack
				UBlackboardComponent* blackboardComponent = OwnerComp.GetBlackboardComponent();
				bool canAttack = blackboardComponent->GetValueAsBool(GetSelectedBlackboardKey());
				if (!canAttack)
				{
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}

				FVector enemyLocation = pawn->GetActorLocation();
				UObject* target = blackboardComponent->GetValueAsObject("AttackTarget");
				if (target)
				{
					FVector targetLocation = Cast<AActor>(target)->GetActorLocation();
					FVector directionToTarget = (targetLocation - enemyLocation).GetSafeNormal();
					//todo do smth with magic number
					FVector traceEnd = enemyLocation + directionToTarget * 3000.0f;
					FRotator newRotation = directionToTarget.Rotation();
					pawn->SetActorRotation(newRotation);
					combatComp->CastSpell(ElementType::None, traceEnd);
				}
			}
		}
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
