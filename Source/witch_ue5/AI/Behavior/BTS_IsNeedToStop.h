#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_IsNeedToStop.generated.h"

UCLASS()
class WITCH_UE5_API UBTS_IsNeedToStop : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTS_IsNeedToStop();
	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
};
