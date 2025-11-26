#include "Actors/BasicLootSpawner.h"
#include "Actors/HealItem.h"
#include "GameFramework/Character.h"
#include "Static/GameStateManager.h"
#include "Kismet/GameplayStatics.h"

ABasicLootSpawner::ABasicLootSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABasicLootSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ABasicLootSpawner::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && !_isOverlapping)
	{
		if (_playerCharacter)
		{
			_isOverlapping = true;
			if (_lootAnimationComponent)
			{
				_lootAnimationComponent->SetOverlap(true);
			}
		}
	}
}

void ABasicLootSpawner::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	_isOverlapping = false;
	if (_lootAnimationComponent)
	{
		_lootAnimationComponent->SetOverlap(false);
	}
}

void ABasicLootSpawner::OnLootItemCollected(AActor* CollectedLootItem)
{
	AHealItem* healItem = Cast<AHealItem>(CollectedLootItem);
	if (healItem)
	{
		healItem->AddHeal();
	}
	else
	{
		if (_lootManager)
		{
			_lootManager->AddLoot(1);
		}
	}
}

void ABasicLootSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasicLootSpawner::SpawnLoot(FVector spawnLocation)
{
	OnSpawn();

	//magic fix
	spawnLocation.Z += 40.f;
	SetActorLocation(spawnLocation);

	for (_spawnedCount; _spawnedCount <= _spawnedCountMax; _spawnedCount++)
	{
		FActorSpawnParameters SpawnParams;
		AActor* lootItem = GetWorld()->SpawnActor<AActor>(LootItem, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
		if (lootItem)
		{
			OnSpawnLootItem(lootItem);
		}
	}

	if (NeedCheckHeal)
	{
		CheckForHealSpawn();
	}

	if (_lootManager)
	{
		_lootManager->AddDestroy(1);
	}

	_lootAnimationComponent->OnLootItemCollect.AddUniqueDynamic(this, &ABasicLootSpawner::OnLootItemCollected);
	_lootAnimationComponent->OnLootDestroyDelegate.AddUniqueDynamic(this, &ABasicLootSpawner::OnLootDestroy);
	_lootAnimationComponent->SetAnimate(true);
	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EBasicSoundType::LootSpawn, GetActorLocation());
}

void ABasicLootSpawner::OnSpawn()
{
	_lootManager = Cast<AGameStateManager>(GetWorld()->GetGameState())->GetLootManager();
	_lootAnimationComponent = FindComponentByClass<ULootAnimationComponent>();
	_playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	_triggerBox = FindComponentByClass<UBoxComponent>();

	if (_triggerBox)
	{
		_triggerBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABasicLootSpawner::OnBeginOverlap);
		_triggerBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ABasicLootSpawner::OnEndOverlap);
	}

	_spawnedCountMax = FMath::RandRange(SpawnCountRandomMin, SpawnCountRandomMax);
}

void ABasicLootSpawner::OnLootDestroy()
{
	Destroy();
}

void ABasicLootSpawner::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (UStaticMeshComponent* lootMesh = Cast<UStaticMeshComponent>(HitComponent))
	{
		if (!lootMesh->IsSimulatingPhysics())
		{
			return;
		}
		lootMesh->SetSimulatePhysics(false);
	}
}

void ABasicLootSpawner::CheckForHealSpawn()
{
	if (!_lootManager)
	{
		return;
	}

	if (_lootManager->CanSpawnHealItem())
	{
		FActorSpawnParameters SpawnParams;
		AActor* lootItem = GetWorld()->SpawnActor<AActor>(HealItem, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
		if (lootItem)
		{
			OnSpawnLootItem(lootItem);
		}
	}
}

void ABasicLootSpawner::OnSpawnLootItem(AActor* lootItem)
{
	_lootAnimationComponent->AddLootItem(lootItem);
	UStaticMeshComponent* lootMesh = lootItem->FindComponentByClass<UStaticMeshComponent>();
	if (lootMesh)
	{
		lootMesh->OnComponentHit.AddDynamic(this, &ABasicLootSpawner::OnHit);
	}
}

