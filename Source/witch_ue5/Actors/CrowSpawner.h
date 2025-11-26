#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "CrowSpawner.generated.h"

UCLASS()
class WITCH_UE5_API ACrowSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ACrowSpawner();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	void SpawnCrows();
	void DestroyCrow();
	void AddCrow();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 MaxSpawnCount = 21;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 MaxSpawnPoints = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 MaxSpawnCountPerPoint = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TSubclassOf<class AActor> Crow;
	
private:
	UNavigationSystemV1* _navSystem;
	TArray<FVector> _randomPoints;
	int32 _currentCount = 0;

	void SpawnCrowsInPoint(FVector point, int32 count);
	void SpawnCrow(FVector location);
	FVector GetRandomPoint();

};
