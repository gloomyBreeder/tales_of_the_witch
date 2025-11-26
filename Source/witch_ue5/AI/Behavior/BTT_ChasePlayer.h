#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_ChasePlayer.generated.h"

UCLASS()
class WITCH_UE5_API UBTT_ChasePlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_ChasePlayer(FObjectInitializer const& ObjectInit);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase", meta = (AllowPrivateAccess = "true"))
	float RadiusFromPlayerToStop = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase", meta = (AllowPrivateAccess = "true"))
	float RecalculateDistanceThreshold = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chase", meta = (AllowPrivateAccess = "true"))
	float MaxDistanceToPlayer = 150.f;
	
};
