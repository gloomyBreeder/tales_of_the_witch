#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NavigationSystem.h"
#include "Actors/CrowSpawner.h"
#include "CrowController.generated.h"

UCLASS()
class WITCH_UE5_API ACrowController : public AAIController
{
	GENERATED_BODY()

public:
	explicit ACrowController(FObjectInitializer const& ObjectInit);

	void BeginPlay() override;
	void OnPossess(APawn* pawn) override;
	void SetSpawnerRef(ACrowSpawner* spawner);

	UFUNCTION()
	void OnTargetDetected(AActor* actor, FAIStimulus const stimulus);

	UFUNCTION(BlueprintCallable)
	bool IsFlying();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight")
	float SightRadius = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	float SearchRadius = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight")
	float SightRangeFromLastSeenLoc = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight")
	float SightOffset = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	float WaitDurationMin = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search")
	float WaitDurationMax = 10.f;
	
private:
	void RandomWalk();
	bool FindSafeFlyingLocation(FVector& outLocation);
	void DestroyCrow();
	void SetupPerceptionSystem();

	class UAISenseConfig_Sight* _sightConfig;
	UNavigationSystemV1* _navSys;
	FTimerHandle _timerHandle;
	ACrowSpawner* _spawner;
	bool _isFlying = false;
};
