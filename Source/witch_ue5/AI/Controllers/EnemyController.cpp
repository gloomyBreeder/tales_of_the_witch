#include "EnemyController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputPlayer/InputCharacter.h"

AEnemyController::AEnemyController(FObjectInitializer const& objectInit)
{
	BehaviorTreeComponent = objectInit.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTree Component"));
	_blackBoard = objectInit.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("Blackboard Component"));
	SetupPerceptionSystem();
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	_combatDirector = Cast<AGameStateManager>(GetWorld()->GetGameState())->GetCombatDirector();
	RunBehaviorTree(BehaviorTree);
	BehaviorTreeComponent->StartTree(*BehaviorTree);
	_navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (_navSys)
	{
		_recastNavMesh = Cast<ARecastNavMesh>(_navSys->GetDefaultNavDataInstance());
	}
}

void AEnemyController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyController::SaveValidHomePosition, 3.f, false);
	if (_blackBoard)
	{
		_blackBoard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	}
}

void AEnemyController::SetAggroState()
{
	_isInAggro = true;
	_blackBoard->SetValueAsBool("CanSeePlayer", true);
}

UBlackboardComponent* AEnemyController::GetBlackboard() const
{
	return _blackBoard;
}

bool AEnemyController::IsInAggro()
{
	return _isInAggro;
}

bool AEnemyController::CanAttack()
{
	return _isTiedToNavArea ? !_combatDirector->IsEnemyTypeInAttack(CombatEnemyType) : true;
}

AActor* AEnemyController::GetAttackTarget()
{
	return _attackTarget;
}

bool AEnemyController::CanGoToLocation(const FNavLocation& navLoc)
{
	if (_isTiedToNavArea)
	{
		uint32 areaID = _recastNavMesh->GetPolyAreaID(navLoc.NodeRef);
		const UClass* foundAreaClass = _recastNavMesh->GetAreaClass(areaID);

		return foundAreaClass == _navArea;
	}

	return true;
}

FVector AEnemyController::GetHomePosition()
{
	return _homePosition;
}

void AEnemyController::AssignArea(UClass* navArea)
{
	_isTiedToNavArea = true;
	_navArea = navArea;
}

void AEnemyController::SaveValidHomePosition()
{
	if (GetPawn())
	{
		_homePosition = GetPawn()->GetActorLocation();
	}
}

void AEnemyController::SetupPerceptionSystem()
{
	_sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (_sightConfig)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		_sightConfig->SightRadius = SightRadius;
		_sightConfig->LoseSightRadius = SightRadius + SightOffset;
		_sightConfig->PeripheralVisionAngleDegrees = 90.f;
		_sightConfig->SetMaxAge(5.f);
		_sightConfig->AutoSuccessRangeFromLastSeenLocation = SightRangeFromLastSeenLoc;
		_sightConfig->DetectionByAffiliation.bDetectEnemies = true;
		_sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		_sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		GetPerceptionComponent()->SetDominantSense(*_sightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*_sightConfig);
	}
}

void AEnemyController::OnTargetDetected(AActor* actor, FAIStimulus const stimulus)
{
	if (Cast<AInputCharacter>(actor))
	{
		_isInAggro = stimulus.WasSuccessfullySensed();
		_blackBoard->SetValueAsBool("CanSeePlayer", _isInAggro);
		_attackTarget = _isInAggro ? actor : nullptr;
		_blackBoard->SetValueAsObject("AttackTarget", _attackTarget);
	}
}
