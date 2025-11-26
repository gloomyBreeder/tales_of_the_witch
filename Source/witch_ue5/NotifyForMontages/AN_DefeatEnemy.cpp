#include "AN_DefeatEnemy.h"
#include "Actors/BasicLootSpawner.h"
#include "AI/Controllers/EnemyController.h"

void UAN_DefeatEnemy::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* owner = MeshComp->GetOwner();

	if (owner)
	{
		// ue is weird, asking meshcomp like oh well sir tell me please about the entire world cause notifies working here dont know about it
		UWorld* world = MeshComp->GetWorld();
		if (world)
		{
			AEnemyController* enemyController = Cast<AEnemyController>(Cast<ACharacter>(owner)->GetController());

			if (enemyController && enemyController->LootSpawner)
			{
				FVector ownerLocation = owner->GetActorLocation();
				FActorSpawnParameters SpawnParams;
				AActor* lootSpawner = world->SpawnActor<AActor>(enemyController->LootSpawner, ownerLocation, FRotator::ZeroRotator, SpawnParams);
				if (lootSpawner)
				{
					(Cast<ABasicLootSpawner>(lootSpawner))->SpawnLoot(ownerLocation);
				}

				owner->K2_DestroyActor();
			}
		}
	}
}
