#include "Actors/DestructibleObject.h"
#include "Static/GameStateManager.h"

ADestructibleObject::ADestructibleObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADestructibleObject::OnBreakEvent(const FChaosBreakEvent& Event)
{
	_breakingFragments++;

	if (!_wasSpawned && _geometryCollection->IsRootBroken())
	{
		_wasSpawned = true;
		Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(FName("Destruct"), BreakSound, GetActorLocation());
		FActorSpawnParameters SpawnParams;
		AActor* lootSpawner = GetWorld()->SpawnActor<AActor>(LootSpawner, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
		if (lootSpawner)
		{
			(Cast<ABasicLootSpawner>(lootSpawner))->SpawnLoot(GetActorLocation());
			// change profile to not collide with debris
			
			_geometryCollection->SetCollisionProfileName(FName("Debris"), true);
			//_geometryCollection->RecreatePhysicsState();
			_geometryCollection->SetCollisionObjectType(ECC_PhysicsBody);
			_geometryCollection->SetMobility(EComponentMobility::Movable);
			_geometryCollection->SetSimulatePhysics(true);
		}
	}
}

void ADestructibleObject::BeginPlay()
{
	Super::BeginPlay();

	_geometryCollection = FindComponentByClass<UGeometryCollectionComponent>();
	if (_geometryCollection)
	{
		_geometryCollection->OnChaosBreakEvent.AddDynamic(this, &ADestructibleObject::OnBreakEvent);
	}
	
}

void ADestructibleObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

