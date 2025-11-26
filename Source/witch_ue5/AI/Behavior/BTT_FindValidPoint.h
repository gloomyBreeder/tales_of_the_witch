#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "NavigationSystem.h"
#include "BTT_FindValidPoint.generated.h"

UCLASS()
class WITCH_UE5_API UBTT_FindValidPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTT_FindValidPoint(FObjectInitializer const& ObjectInit);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Radius = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float KeepOutFromPlayerRadius = 500.0f;

private:
	bool FindIfRandomPointIsValid(const FVector& PlayerLocation, APawn* AIPawn, FVector& OutPoint);
	bool IsPointValid(const FVector& PlayerLocation, APawn* AIPawn, FVector& Point);
	bool PathGoesTooCloseToPlayer(const FVector& PlayerLocation, APawn* AIPawn, const FVector& Point);

	UNavigationSystemV1* _navSys;
	
};
