#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorComponents/CombatComponent.h"
#include "Combat/CombatStructs.h"
#include "EnemyCombatDirector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyKill, AActor*, Actor);

UCLASS()
class WITCH_UE5_API AEnemyCombatDirector : public AActor
{
	GENERATED_BODY()
	
public:
	AEnemyCombatDirector();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FEnemyKill OnEnemyKill;

	void AddEnemy(AActor* enemy);
	void RemoveEnemy(UCombatComponent* combatComp);
	void CheckEnemiesInAttack();
	bool IsEnemyTypeInAttack(CombatEnemyType enemyType);
	void SpawnEnemies(FVector location, FEnemySpawnSettings spawnSettings);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> EnemySpawnerActor;

private:
	void RemoveEnemy(UCombatComponent* combatComp, AEnemyController* enemyController, TArray<FEnemyCombatData>& enemies);
	bool IsEnemyTypeInAttack(TArray<FEnemyCombatData>& enemies);

	// 2 arrays for easier control because i want to divivde attack state for different groups of enemies
	TArray<FEnemyCombatData> _enemiesMelee;
	TArray<FEnemyCombatData> _enemiesRanged;
	TMap<CombatEnemyType, bool> _enemiesInAttack;

};
