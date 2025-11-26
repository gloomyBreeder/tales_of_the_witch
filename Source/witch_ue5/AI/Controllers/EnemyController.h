#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Combat/CombatStructs.h"
#include "Runtime/Engine/Classes/AI/Navigation/NavigationTypes.h"
#include "NavigationSystem.h"
#include "Runtime/NavigationSystem/Public/NavMesh/RecastNavMesh.h"
#include "Actors/EnemyCombatDirector.h"
#include "Static/GameStateManager.h"
#include "EnemyController.generated.h"

UCLASS()
class WITCH_UE5_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	explicit AEnemyController(FObjectInitializer const& ObjectInit);

	void BeginPlay() override;
	void OnPossess(APawn* pawn) override;
	void SetAggroState();
	bool IsInAggro();
	bool CanAttack();
	bool CanGoToLocation(const FNavLocation& navLoc);
	FVector GetHomePosition();
	void AssignArea(UClass* navArea);
	class UBlackboardComponent* GetBlackboard() const;

	UFUNCTION(BlueprintCallable)
	AActor* GetAttackTarget();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Enemy Type")
	CombatEnemyType CombatEnemyType = CombatEnemyType::Melee;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Spawner")
	TSubclassOf<AActor> LootSpawner;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBehaviorTree* BehaviorTree;

private:
	class UBlackboardComponent* _blackBoard;
	class UAISenseConfig_Sight* _sightConfig;

	void SaveValidHomePosition();
	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* actor, FAIStimulus const stimulus);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight", meta = (AllowPrivateAccess = "true"))
	float SightRadius = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight", meta = (AllowPrivateAccess = "true"))
	float SightRangeFromLastSeenLoc = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight", meta = (AllowPrivateAccess = "true"))
	float SightOffset = 500.f;

	bool _isInAggro;
	bool _isTiedToNavArea = false;
	AActor* _attackTarget;
	AEnemyCombatDirector* _combatDirector;
	UClass* _navArea;
	UNavigationSystemV1* _navSys;
	ARecastNavMesh* _recastNavMesh;
	FVector _homePosition;
};
