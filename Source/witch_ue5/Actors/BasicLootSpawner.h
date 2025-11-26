#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Static/LootManager.h"
#include "ActorComponents/LootAnimationComponent.h"
#include "BasicLootSpawner.generated.h"

UCLASS()
class WITCH_UE5_API ABasicLootSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ABasicLootSpawner();
	virtual void Tick(float DeltaTime) override;

	void SpawnLoot(FVector spawnLocation);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Spawner")
	TSubclassOf<AActor> LootItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Spawner")
	TSubclassOf<AActor> HealItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Spawner")
	int SpawnCountRandomMin = 8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Spawner")
	int SpawnCountRandomMax = 14;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Spawner")
	bool NeedCheckHeal = true;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnLootItemCollected(AActor* CollectedLootItem);
	UFUNCTION()
	void OnLootDestroy();
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	void OnSpawn();
	void CheckForHealSpawn();
	void OnSpawnLootItem(AActor* lootItem);

	UBoxComponent* _triggerBox;
	ULootAnimationComponent* _lootAnimationComponent;
	ULootManager* _lootManager;
	ACharacter* _playerCharacter;
	bool _isOverlapping = false;
	int32 _spawnedCount = 0;
	int32 _spawnedCountMax = 0;

};
