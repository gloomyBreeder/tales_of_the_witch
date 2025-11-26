#include "Actors/HealItem.h"
#include "Kismet/GameplayStatics.h"
#include "ActorComponents/HealComponent.h"
#include "GameFramework/Character.h"
#include "Static/GameStateManager.h"
#include "Sound/SoundManager.h"
#include "HealItem.h"

AHealItem::AHealItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHealItem::AddHeal()
{
	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (player)
	{
		UHealComponent* healComponent = player->FindComponentByClass<UHealComponent>();
		if (healComponent)
		{
			Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EBasicSoundType::GetHeal, GetActorLocation());
			healComponent->AddHeal();
		}
	}
}

void AHealItem::BeginPlay()
{
	Super::BeginPlay();
}

void AHealItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

