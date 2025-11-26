#include "AI/Controllers/CrowController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Static/GameStateManager.h"
#include "Perception/AIPerceptionComponent.h"

ACrowController::ACrowController(FObjectInitializer const& ObjectInit)
{
	_sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
}

void ACrowController::BeginPlay()
{
	Super::BeginPlay();
	_navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	SetupPerceptionSystem();
	float randomWaitDuration = FMath::FRandRange(WaitDurationMin, WaitDurationMax);
	GetWorldTimerManager().SetTimer(_timerHandle, this, &ACrowController::RandomWalk, randomWaitDuration, true);
}

void ACrowController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);
}

void ACrowController::OnTargetDetected(AActor* actor, FAIStimulus const stimulus)
{
	ACharacter* character = Cast<ACharacter>(actor);
	if (character)
	{
		//ignore crows
		if (Cast<ACrowController>(character->GetController()))
		{
			return;
		}

		if (stimulus.WasSuccessfullySensed())
		{
			UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetPawn()->GetMovementComponent());
			if (MovementComponent)
			{
				GetWorldTimerManager().ClearTimer(_timerHandle);
				MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
				_isFlying = true;
				FVector SafeLocation;
				if (FindSafeFlyingLocation(SafeLocation))
				{
					Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EBasicSoundType::BirdFly, GetPawn()->GetActorLocation());
					MoveToLocation(SafeLocation, (-1.f), false, false);
				}

				int32 randomTime = FMath::RandRange(15, 20);
				GetWorld()->GetTimerManager().SetTimer(_timerHandle, this, &ACrowController::DestroyCrow, randomTime, false);
			}
		}
	}
}

bool ACrowController::IsFlying()
{
	return _isFlying;
}

void ACrowController::SetSpawnerRef(ACrowSpawner* spawner)
{
	_spawner = spawner;
}

bool ACrowController::FindSafeFlyingLocation(FVector& outLocation)
{
	FVector startLocation = GetPawn()->GetActorLocation();

	for (int i = 0; i < 10; i++)
	{
		FVector testLocation = startLocation + FVector(FMath::RandRange(1000.f, 2000.f),
			FMath::RandRange(-1000.f, 1000.f),
			FMath::RandRange(6000.f, 9000.f));

		// check if no obstacles around
		FHitResult hit;
		if (!GetWorld()->LineTraceSingleByChannel(hit, testLocation, testLocation - FVector(0, 0, 500), ECC_Visibility))
		{
			outLocation = testLocation;
			return true;
		}
	}

	return false;
}

void ACrowController::DestroyCrow()
{
	if (GetPawn())
	{
		if (_spawner)
		{
			_spawner->DestroyCrow();
		}

		GetPawn()->Destroy();
	}
}

void ACrowController::RandomWalk()
{
	FNavLocation location;
	if (_navSys->GetRandomPointInNavigableRadius(GetPawn()->GetActorLocation(), SearchRadius, location))
	{
		MoveToLocation(location.Location);
	}
}

void ACrowController::SetupPerceptionSystem()
{
	if (_sightConfig)
	{
		_sightConfig->SightRadius = SightRadius;
		_sightConfig->PeripheralVisionAngleDegrees = 90.f;
		_sightConfig->SetMaxAge(1.f);
		_sightConfig->LoseSightRadius = SightRadius + SightOffset;
		_sightConfig->AutoSuccessRangeFromLastSeenLocation = SightRangeFromLastSeenLoc;
		_sightConfig->DetectionByAffiliation.bDetectEnemies = true;
		_sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		_sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		GetPerceptionComponent()->SetDominantSense(*_sightConfig->GetSenseImplementation());
		if (!GetPerceptionComponent()->OnTargetPerceptionUpdated.IsAlreadyBound(this, &ACrowController::OnTargetDetected))
		{
			GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ACrowController::OnTargetDetected);
		}
		GetPerceptionComponent()->ConfigureSense(*_sightConfig);
	}
}
