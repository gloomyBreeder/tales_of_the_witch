#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_IsInValidPointNearThePlayer.generated.h"

UCLASS()
class WITCH_UE5_API UBTS_IsInValidPointNearThePlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTS_IsInValidPointNearThePlayer();
	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MinimumDistance = 200.0f;

private:
	bool IsPointValid(const FVector& PlayerLocation, APawn* AIPawn, FVector& Point);
	
};
