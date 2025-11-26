#include "AI/Behavior/BTT_Focus.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BTT_Focus.h"

UBTT_Focus::UBTT_Focus(FObjectInitializer const& ObjectInit)
{
	NodeName = TEXT("Focus");
}

EBTNodeResult::Type UBTT_Focus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* player = playerController->GetPawn();

    if (player)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), player);
        AIController->SetFocus(player, EAIFocusPriority::Gameplay);
        return EBTNodeResult::Succeeded;
    }
    else
    {
        AIController->ClearFocus(EAIFocusPriority::Gameplay);
        return EBTNodeResult::Failed;
    }
}
