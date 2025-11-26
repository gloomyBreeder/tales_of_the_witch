#include "Actors/CrowSpawner.h"
#include "GameFramework/Character.h"
#include "AI/Controllers/CrowController.h"

ACrowSpawner::ACrowSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACrowSpawner::SpawnCrows()
{
	_navSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	for (int i = 0; i < MaxSpawnPoints; i++)
	{
		FNavLocation point;
		if (_navSystem->GetRandomPoint(point))
		{
			_randomPoints.Add(point.Location);
		}
	}

	for (FVector& point : _randomPoints)
	{
		if (_currentCount >= MaxSpawnCount)
		{
			break;
		}

		int32 countToSpawn = FMath::RandRange(1, MaxSpawnCountPerPoint);
		countToSpawn = FMath::Min(countToSpawn, MaxSpawnCount - _currentCount);

		if (countToSpawn > 0)
		{
			SpawnCrowsInPoint(point, countToSpawn);
		}
	}

	while (_currentCount < MaxSpawnCount)
	{
		SpawnCrowsInPoint(GetRandomPoint(), 1);
	}
}

void ACrowSpawner::SpawnCrowsInPoint(FVector point, int32 count)
{
	for (int i = 0; i < count; i++)
	{
		if (_currentCount >= MaxSpawnCount)
		{
			break;
		}

		//small radius to look more natural for birds
		FNavLocation location;
		if (_navSystem->GetRandomPointInNavigableRadius(point, 100.f, location))
		{
			SpawnCrow(location.Location);
		}
	}
}

void ACrowSpawner::SpawnCrow(FVector location)
{
	AActor* crow = GetWorld()->SpawnActor<AActor>(Crow);
	if (crow)
	{
		//very cool magic fix
		location.Z += 40;
		crow->SetActorLocation(location);
		ACharacter* character = Cast<ACharacter>(crow);
		if (character)
		{
			ACrowController* controller = Cast<ACrowController>(character->GetController());
			if (controller)
			{
				controller->SetSpawnerRef(this);
			}
		}
		_currentCount++;
	}
}

FVector ACrowSpawner::GetRandomPoint()
{
	if (_randomPoints.Num() == 0)
	{
		return FVector::ZeroVector;
	}

	int32 randomNumber = FMath::RandRange(0, _randomPoints.Num() - 1);
	return _randomPoints[randomNumber];
}

void ACrowSpawner::DestroyCrow()
{
	_currentCount--;
	if (_currentCount < MaxSpawnCount)
	{
		FVector point = GetRandomPoint();
		SpawnCrowsInPoint(point, 1);
	}
}

//use it to add crows manually if needed
void ACrowSpawner::AddCrow()
{
	_currentCount++;
}

void ACrowSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ACrowSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACrowSpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SpawnCrows();
}

