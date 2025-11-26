#include "ActorComponents/CollectibleComponent.h"

UCollectibleComponent::UCollectibleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCollectibleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCollectibleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

