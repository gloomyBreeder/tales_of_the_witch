#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Actors/BasicLootSpawner.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "DestructibleObject.generated.h"

UCLASS()
class WITCH_UE5_API ADestructibleObject : public AActor
{
	GENERATED_BODY()
	
public:	
	ADestructibleObject();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* BreakSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Spawner")
	TSubclassOf<AActor> LootSpawner;
	UFUNCTION()
	void OnBreakEvent(const FChaosBreakEvent& Event);

private:
	UGeometryCollectionComponent* _geometryCollection;
	ABasicLootSpawner* _lootSpawner;
	int32 _breakingFragments = 0;
	bool _wasSpawned = false;

};
