#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_IsPlayerInRange.generated.h"

UCLASS()
class WITCH_UE5_API UBTS_IsPlayerInRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTS_IsPlayerInRange();

	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Range = 100.0f;

private:
	bool _isCached = false;
	bool _useControllerDesiredRotation;
	bool _orientRotationToMovement;
	bool _useControllerRotationYaw;
};
