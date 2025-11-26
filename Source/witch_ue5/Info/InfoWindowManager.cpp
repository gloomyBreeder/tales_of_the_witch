#include "InfoWindowManager.h"

AInfoWindowManager::AInfoWindowManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AInfoWindowManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInfoWindowManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

