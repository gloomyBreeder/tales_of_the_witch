#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MoveToWithStop.generated.h"

UCLASS()
class WITCH_UE5_API UBTT_MoveToWithStop : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_MoveToWithStop(FObjectInitializer const& ObjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
