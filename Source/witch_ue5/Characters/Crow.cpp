#include "Characters/Crow.h"

ACrow::ACrow()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACrow::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACrow::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

