#include "Actors/EnemyCombatDirector.h"
#include "AI/Controllers/EnemyController.h"
#include "Actors/EnemySpawner.h"


// todo ADD TEMPLATES REFACTORING
AEnemyCombatDirector::AEnemyCombatDirector()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCombatDirector::BeginPlay()
{
	Super::BeginPlay();

	_enemiesInAttack.Add(CombatEnemyType::Melee, false);
	_enemiesInAttack.Add(CombatEnemyType::Magic, false);
}

void AEnemyCombatDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckEnemiesInAttack();
}

void AEnemyCombatDirector::AddEnemy(AActor* enemy)
{
	UCombatComponent* combatComp = enemy->FindComponentByClass<UCombatComponent>();
	if (combatComp)
	{
		AEnemyController* enemyController = Cast<AEnemyController>(Cast<ACharacter>(enemy)->GetController());
		FEnemyCombatData combatData(combatComp, enemyController);
		if (enemyController->CombatEnemyType == CombatEnemyType::Melee)
		{
			_enemiesMelee.Add(combatData);
		}
		else if (enemyController->CombatEnemyType == CombatEnemyType::Magic)
		{
			_enemiesRanged.Add(combatData);
		}
	}
}

void AEnemyCombatDirector::RemoveEnemy(UCombatComponent* combatComp)
{
	AEnemyController* enemyController = Cast<AEnemyController>(Cast<ACharacter>(combatComp->GetOwner())->GetController());
	if (enemyController->CombatEnemyType == CombatEnemyType::Melee)
	{
		RemoveEnemy(combatComp, enemyController, _enemiesMelee);
	}
	else if (enemyController->CombatEnemyType == CombatEnemyType::Magic)
	{
		RemoveEnemy(combatComp, enemyController, _enemiesRanged);
	}

	CheckEnemiesInAttack();
}

void AEnemyCombatDirector::RemoveEnemy(UCombatComponent* combatComp, AEnemyController* enemyController, TArray<FEnemyCombatData>& enemies)
{
	if (enemies.Num() > 0)
	{
		for (int32 i = enemies.Num() - 1; i >= 0; --i)
		{
			FEnemyCombatData enemy = enemies[i];
			if (enemy.CombatComponent == combatComp && enemy.EnemyController == enemyController)
			{
				OnEnemyKill.Broadcast(enemy.CombatComponent->GetOwner());
				enemies.RemoveAt(i);
				break;
			}
		}
	}
}

void AEnemyCombatDirector::CheckEnemiesInAttack()
{
	if (_enemiesInAttack.Num() > 0)
	{
		for (TPair<CombatEnemyType, bool>& pair : _enemiesInAttack)
		{
			if (pair.Key == CombatEnemyType::Melee)
			{
				bool isEnemyTypeInAttack = IsEnemyTypeInAttack(_enemiesMelee);
				if (pair.Value != isEnemyTypeInAttack)
				{
					pair.Value = isEnemyTypeInAttack;
				}
			}
			else if (pair.Key == CombatEnemyType::Magic)
			{
				bool isEnemyTypeInAttack = IsEnemyTypeInAttack(_enemiesRanged);
				if (pair.Value != isEnemyTypeInAttack)
				{
					pair.Value = isEnemyTypeInAttack;
				}
			}
		}
	}
}

bool AEnemyCombatDirector::IsEnemyTypeInAttack(TArray<FEnemyCombatData>& enemies)
{
	if (enemies.Num() > 0)
	{
		for (FEnemyCombatData enemy : enemies)
		{
			if (enemy.CombatComponent->IsAttacking() || enemy.CombatComponent->IsCasting())
			{
				return true;
			}
		}
	}

	return false;
}

bool AEnemyCombatDirector::IsEnemyTypeInAttack(CombatEnemyType enemyType)
{
	if (_enemiesInAttack.Num() > 0)
	{
		for (TPair<CombatEnemyType, bool> pair : _enemiesInAttack)
		{
			if (pair.Key == enemyType)
			{
				return IsEnemyTypeInAttack(enemyType == CombatEnemyType::Melee ? _enemiesMelee : _enemiesRanged);
			}
		}
	}

	return false;
}

void AEnemyCombatDirector::SpawnEnemies(FVector location, FEnemySpawnSettings spawnSettings)
{
	if (EnemySpawnerActor)
	{
		FActorSpawnParameters spawnParams;
		AActor* enemySpawnerActor = GetWorld()->SpawnActor<AActor>(EnemySpawnerActor, location, FRotator::ZeroRotator, spawnParams);
		if (enemySpawnerActor)
		{
			AEnemySpawner* enemySpawner = Cast<AEnemySpawner>(enemySpawnerActor);
			if (enemySpawner)
			{
				enemySpawner->SetSettings(spawnSettings);
				enemySpawner->Init();
			}
		}
	}
}

