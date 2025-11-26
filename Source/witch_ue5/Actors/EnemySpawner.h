#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Combat/CombatStructs.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Actors/EnemyCombatDirector.h"
#include "EnemySpawner.generated.h"

UCLASS()
class WITCH_UE5_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();
	virtual void Tick(float DeltaTime) override;

	void SetSettings(FEnemySpawnSettings settings);
	void Init();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	FEnemySpawnSettings SpawnSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	AActor* NavVolumeActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	UClass* NavArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 MaxAttempts = 100;

private:
	void SetEnemyInAttackMode(AActor* enemy);
	void Spawn(FVector location, FEnemySpawnTypeDyn& randomEnemy);
	void SpawnEnemies(int32 count);
	FEnemySpawnTypeDyn& GetRandomEnemySpawnType();
	void SetAllEnemiesInAttackMode();
	FEnemySpawnType* GetCostEnemy(TArray<FEnemySpawnType*>& allRows);

	UPROPERTY()
	TArray<FEnemySpawnTypeDyn> _enemies;

	TArray<AActor*> _enemiesActors;
	UNavigationSystemV1* _navSystem;
	AEnemyCombatDirector* _combatDirector;
	bool _inited = false;
	bool _isAnyEnemyAttacking = false;
	float _elapsedTime = 0.0f;

	int32 _spawnPerWave = 0;
	int32 _totalCount = 0;
	int32 _totalCostRemainder = 0;
	int32 _wavesCount;
};
