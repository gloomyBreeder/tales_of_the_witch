#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindPlayerLocation.generated.h"

UCLASS()
class WITCH_UE5_API UBTT_FindPlayerLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_FindPlayerLocation(FObjectInitializer const& ObjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float SearchRadius = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	bool SearchRandom = false;
	
};
