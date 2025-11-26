#pragma once

#include "CombatStructs.generated.h"

class AEnemyController;
class UCombatComponent;

USTRUCT(BlueprintType)
struct FAttack
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackMontage;
};

UENUM(BlueprintType)
enum class EEnemyRarity : uint8
{
	Common = 0 UMETA(DisplayName = "Common"),
	Uncommon = 1 UMETA(DisplayName = "Uncommon"),
	Rare = 2 UMETA(DisplayName = "Rare"),
};

UENUM(BlueprintType)
enum class ElementType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Fire = 1 UMETA(DisplayName = "Fire"),
	Water = 2 UMETA(DisplayName = "Water"),
	Lightning = 3 UMETA(DisplayName = "Lightning"),
	Necro = 4 UMETA(DisplayName = "Necro"),
};

//order elements for player
static TArray<ElementType> ElementOrder = { ElementType::Fire, ElementType::Water, ElementType::Lightning };

UENUM(BlueprintType)
enum class CombatEnemyType : uint8
{
	Melee = 0 UMETA(DisplayName = "Melee"),
	Ranged = 1 UMETA(DisplayName = "Ranged"),
	Magic = 2 UMETA(DisplayName = "Magic")
};

USTRUCT(BlueprintType)
struct FAttackStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ElementType DamageType;
};

USTRUCT(BlueprintType)
struct FEnemySpawnSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 TotalCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TArray<int32> EnemyTableKeys;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	float SpawnRadius = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	float DelayBetweenWaves = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 WavesCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	bool SpawnInsideNavArea = true;
};

USTRUCT(BlueprintType)
struct FEnemySpawnWave
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 SpawnPerWave;
};

USTRUCT(BlueprintType)
struct FEnemySpawnType : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 TableKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	EEnemyRarity EnemyRarity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	int32 EnemyCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TSubclassOf<class AActor> Enemy;
};

USTRUCT(BlueprintType)
struct FEnemySpawnTypeDyn
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemySpawnType EnemySpawnType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EnemySpawnCount;

	FEnemySpawnTypeDyn() : EnemySpawnType(), EnemySpawnCount(0)
	{
	}

	FEnemySpawnTypeDyn(const FEnemySpawnType& InEnemySpawnType, int32 EnemySpawnCount) : EnemySpawnType(InEnemySpawnType), EnemySpawnCount(EnemySpawnCount)
	{
	}
};

USTRUCT(BlueprintType)
struct FParticleProjectile : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ElementType ElementType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AProjectile> Projectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* ParticleSystem;
};

USTRUCT(BlueprintType)
struct FEnemyCombatData
{
	GENERATED_USTRUCT_BODY()

	UCombatComponent* CombatComponent;
	AEnemyController* EnemyController;

	FEnemyCombatData() : CombatComponent(nullptr), EnemyController(nullptr)
	{
	}

	FEnemyCombatData(UCombatComponent* CombatComponent, AEnemyController* EnemyController) : CombatComponent(CombatComponent), EnemyController(EnemyController)
	{
	}
};