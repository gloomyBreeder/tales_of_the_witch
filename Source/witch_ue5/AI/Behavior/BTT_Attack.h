#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_Attack.generated.h"

UCLASS()
class WITCH_UE5_API UBTT_Attack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_Attack(FObjectInitializer const& ObjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Attack;
};
