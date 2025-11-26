#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_Focus.generated.h"

UCLASS()
class WITCH_UE5_API UBTT_Focus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_Focus(FObjectInitializer const& ObjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
