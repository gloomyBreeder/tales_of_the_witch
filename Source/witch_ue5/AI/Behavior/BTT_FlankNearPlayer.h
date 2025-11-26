#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "BTT_FlankNearPlayer.generated.h"

UCLASS()
class WITCH_UE5_API UBTT_FlankNearPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_FlankNearPlayer(FObjectInitializer const& ObjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flank Parameters", meta = (AllowPrivateAccess = "true"))
	float MinThreshold = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flank Parameters", meta = (AllowPrivateAccess = "true"))
	float MaxThreshold = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flank Parameters", meta = (AllowPrivateAccess = "true"))
	float RecalculateDistanceThreshold = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flank Parameters", meta = (AllowPrivateAccess = "true"))
	float DistanceToStopNearPlayer = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flank Parameters", meta = (AllowPrivateAccess = "true"))
	int32 RightVectorMinThreshold = -3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flank Parameters", meta = (AllowPrivateAccess = "true"))
	int32 RightVectorMaxThreshold = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flank Parameters", meta = (AllowPrivateAccess = "true"))
	float RightVectorCoeff = 150.f;

private:
	UPROPERTY()
	UBehaviorTreeComponent* _cachedOwnerComp;
	
};
