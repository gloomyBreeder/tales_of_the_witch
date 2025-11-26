#include "AI/Behavior/BTS_ChangeSpeed.h"
#include "AI/Controllers/EnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "BTS_ChangeSpeed.h"

UBTS_ChangeSpeed::UBTS_ChangeSpeed()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Change Speed");
}

void UBTS_ChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (const ACharacter* character = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		character->GetCharacterMovement()->MaxWalkSpeed = Speed;
	}
}
