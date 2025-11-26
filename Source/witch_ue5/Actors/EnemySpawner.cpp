#include "Actors/EnemySpawner.h"
#include "ActorComponents/CombatComponent.h"
#include "Components/BrushComponent.h"
#include "Static/GameStateManager.h"
#include "Runtime/NavigationSystem/Public/NavMesh/RecastNavMesh.h"
#include "AI/Controllers/EnemyController.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_inited)
	{
		return;
	}

	//remove dead enemies
	if (_enemiesActors.Num() > 0)
	{
		for (int32 i = _enemiesActors.Num() - 1; i >= 0; --i)
		{
			AActor* enemy = _enemiesActors[i];
			UCombatComponent* combatComp = enemy->FindComponentByClass<UCombatComponent>();
			if (combatComp && (combatComp->IsDead() || combatComp->IsDying()))
			{
				_combatDirector->RemoveEnemy(combatComp);
				_enemiesActors.RemoveAt(i);
			}
		}
	}

	//todo do i need it
	if (!_isAnyEnemyAttacking)
	{
		for (AActor* enemy : _enemiesActors)
		{
			ACharacter* character = Cast<ACharacter>(enemy);
			if (character)
			{
				AEnemyController* controller = Cast<AEnemyController>(character->GetController());
				if (controller && controller->IsInAggro())
				{
					//if any is attacking - every enemy should attack
					_isAnyEnemyAttacking = true;
					break;
				}
			}
		}
	}

	if (_isAnyEnemyAttacking)
	{
		SetAllEnemiesInAttackMode();

		//wait time to spawn next wave
		if (_wavesCount > 0)
		{
			if (_elapsedTime >= SpawnSettings.DelayBetweenWaves)
			{
				_elapsedTime = 0.f;
				SpawnEnemies(_spawnPerWave);
				_wavesCount--;
			}
			else
			{
				_elapsedTime += DeltaTime;
			}
		}
		else
		{
			if (_enemiesActors.Num() == 0)
			{
				Destroy();
			}
		}
	}
}

void AEnemySpawner::Init()
{
	if (_inited)
	{
		return;
	}

	_navSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	_combatDirector = Cast<AGameStateManager>(GetWorld()->GetGameState())->GetCombatDirector();
	_wavesCount = SpawnSettings.WavesCount;

	UDataTable* enemyDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Config/DT_EnemySpawnTypes.DT_EnemySpawnTypes"));
	if (enemyDataTable)
	{
		TArray<FEnemySpawnType*> allRows;
		enemyDataTable->GetAllRows<FEnemySpawnType>("", allRows);
		_totalCostRemainder = SpawnSettings.TotalCost;

		if (allRows.Num() == 0)
		{
			return;
		}

		//test
		for (const auto& EnemyType : allRows)
		{
			if (!EnemyType->Enemy)
			{
				UE_LOG(LogTemp, Error, TEXT("Missing enemy class for row: %d"), EnemyType->TableKey);
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Loaded enemy class: %d"), EnemyType->TableKey);
			}
		}
		//

		for (int32 enemyKey : SpawnSettings.EnemyTableKeys)
		{
			//get enemy count from max cost to min
			if (_totalCostRemainder > 0)
			{
				FEnemySpawnType* foundRow = GetCostEnemy(allRows);
				if (!foundRow)
				{
					break;
				}
				int32 enemiesCount = _totalCostRemainder / foundRow->EnemyCost;
				if (enemiesCount > 0)
				{
					//save data to spawn later (saving copies because GC deleting everything)
					FEnemySpawnTypeDyn spawnTypeData(*foundRow, enemiesCount);
					_enemies.Add(spawnTypeData);
					_totalCount += enemiesCount;
					_totalCostRemainder = _totalCostRemainder % (foundRow->EnemyCost * enemiesCount);
				}
			}
		}

		//how many enemies to spawn in the first wave
		_spawnPerWave = _totalCount / _wavesCount;
		int32 spawnPerWaveRemainder = _totalCount % _wavesCount;
		int32 spawnCount = _spawnPerWave;

		if (spawnPerWaveRemainder > 0)
		{
			// sum this count to first wave 
			spawnCount += spawnPerWaveRemainder;
		}

		SpawnEnemies(spawnCount);
		_wavesCount--;
		_inited = true;
	}
}

void AEnemySpawner::SetSettings(FEnemySpawnSettings settings)
{
	SpawnSettings = settings;
}

void AEnemySpawner::SetEnemyInAttackMode(AActor* enemy)
{
	ACharacter* character = Cast<ACharacter>(enemy);
	if (character)
	{
		AEnemyController* controller = Cast<AEnemyController>(character->GetController());
		if (controller && !controller->IsInAggro())
		{
			controller->SetAggroState();
		}
	}
}

FEnemySpawnType* AEnemySpawner::GetCostEnemy(TArray<FEnemySpawnType*>& allRows)
{
	TArray<FEnemySpawnType*> FilteredRows = allRows.FilterByPredicate([this](const FEnemySpawnType* Enemy)
		{
			return SpawnSettings.EnemyTableKeys.Contains(Enemy->TableKey);
		});

	FilteredRows.Sort([](const FEnemySpawnType& A, const FEnemySpawnType& B)
		{
			return A.EnemyCost > B.EnemyCost;
		});

	if (FilteredRows.Num() > 0)
	{
		FEnemySpawnType* FoundRow = FilteredRows[0];
		allRows.Remove(FoundRow);
		return FoundRow;
	}

	return nullptr;
}

void AEnemySpawner::Spawn(FVector location, FEnemySpawnTypeDyn& randomEnemy)
{
	if (!randomEnemy.EnemySpawnType.Enemy)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "no enemy in enemy spawn type");
		UE_LOG(LogTemp, Error, TEXT("no enemy in enemy spawn type: %d"), randomEnemy.EnemySpawnType.TableKey);
		return;
	}

	//magiiiiic numbeeeers
	location.Z += 100;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* spawnedEnemy = GetWorld()->SpawnActor<AActor>(randomEnemy.EnemySpawnType.Enemy, location, FRotator::ZeroRotator, SpawnParams);
	
	if (spawnedEnemy)
	{
		//spawnedEnemy->SetActorLocation(location);
		if (_isAnyEnemyAttacking)
		{
			SetEnemyInAttackMode(spawnedEnemy);
		}

		//set ref to combat director
		ACharacter* character = Cast<ACharacter>(spawnedEnemy);
		if (character)
		{
			AEnemyController* controller = Cast<AEnemyController>(character->GetController());
			if (controller && SpawnSettings.SpawnInsideNavArea)
			{
				controller->AssignArea(NavArea);
			}
		}

		_enemiesActors.Add(spawnedEnemy);
		_combatDirector->AddEnemy(spawnedEnemy);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "tried to spawn enemy but failed");
	}
}

void AEnemySpawner::SpawnEnemies(int32 count)
{
	ARecastNavMesh* recastNavMesh = Cast<ARecastNavMesh>(_navSystem->GetDefaultNavDataInstance());
	
	for (int i = 0; i < count; i++)
	{
		FEnemySpawnTypeDyn& randomEnemy = GetRandomEnemySpawnType();
		if (!SpawnSettings.SpawnInsideNavArea)
		{
			bool foundLocation = false;
			for (int k = 0; k < MaxAttempts; k++)
			{
				FNavLocation location;
				if (_navSystem->GetRandomPointInNavigableRadius(GetActorLocation(), SpawnSettings.SpawnRadius, location))
				{
					Spawn(location.Location, randomEnemy);
					//decrease count of enemy
					randomEnemy.EnemySpawnCount--;
					foundLocation = true;
					break;
				}
			}

			if (!foundLocation)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "cant GetRandomPointInNavigableRadius");
			}
		}
		else
		{
			UBrushComponent* brushComponent = NavVolumeActor->FindComponentByClass<UBrushComponent>();
			if (!brushComponent)
			{
				return;
			}

			FBoxSphereBounds localBounds = brushComponent->GetLocalBounds();
			FVector localBoxSize = localBounds.BoxExtent * 2;
			FVector scaledBoxSize = localBoxSize * brushComponent->GetComponentScale();
			FVector center = NavVolumeActor->GetActorLocation();
			FVector min = center - (scaledBoxSize * 0.5f);
			FVector max = center + (scaledBoxSize * 0.5f);
			FNavLocation randomNavLocation;
			bool foundLocation = false;

			for (int j = 0; j < MaxAttempts; j++)
			{
				FVector randomPoint = FMath::RandPointInBox(FBox(min, max));
				if (_navSystem->ProjectPointToNavigation(randomPoint, randomNavLocation))
				{
					FVector extent = scaledBoxSize * 0.5f;
					NavNodeRef nearestPoly = recastNavMesh->FindNearestPoly(randomNavLocation.Location, extent);
					if (nearestPoly != INVALID_NAVNODEREF)
					{
						uint32 areaID = recastNavMesh->GetPolyAreaID(nearestPoly);
						const UClass* areaClass = recastNavMesh->GetAreaClass(areaID);
						if (areaClass == NavArea)
						{
							Spawn(randomNavLocation.Location, randomEnemy);
							randomEnemy.EnemySpawnCount--;
							foundLocation = true;
							break;
						}
					}
				}
			}

			if (!foundLocation)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "cant foundLocation for enemy for some reason");
			}
		}
	}
}

FEnemySpawnTypeDyn& AEnemySpawner::GetRandomEnemySpawnType()
{
	TArray<int32> validIndices;
	float totalWeight = 0.0f;

	for (int32 i = 0; i < _enemies.Num(); ++i)
	{
		// get only types with count > 0
		if (_enemies[i].EnemySpawnCount > 0)
		{
			validIndices.Add(i);
			// use weight to understand what type of enemy is better to spawn
			totalWeight += (_enemies[i].EnemySpawnType.Weight * _enemies[i].EnemySpawnCount);
		}
	}

	if (validIndices.Num() == 0)
	{
		throw std::runtime_error("No valid enemies available for spawning.");
	}

	float randomValue = FMath::RandRange(0.0f, totalWeight);
	float cumulativeWeight = 0.0f;

	for (int32 index : validIndices)
	{
		cumulativeWeight += (_enemies[index].EnemySpawnType.Weight * _enemies[index].EnemySpawnCount);
		if (randomValue <= cumulativeWeight)
		{
			return _enemies[index];
		}
	}

	throw std::runtime_error("Failed to select a valid enemy.");
}

void AEnemySpawner::SetAllEnemiesInAttackMode()
{
	for (AActor* enemy : _enemiesActors)
	{
		SetEnemyInAttackMode(enemy);
	}
}

