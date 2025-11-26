#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindRandomLocation.generated.h"

UCLASS()
class WITCH_UE5_API UBTT_FindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTT_FindRandomLocation(FObjectInitializer const& objectInit);
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI View Radius", meta = (AllowPrivateAccess = "true"))
	float SearchRadius = 500.f;
};
