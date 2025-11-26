#include "ActorComponents/TelekinesisMovableComponent.h"

UTelekinesisMovableComponent::UTelekinesisMovableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTelekinesisMovableComponent::BeginPlay()
{
	Super::BeginPlay();
	_beginPosition = GetOwner()->GetActorLocation();
}

void UTelekinesisMovableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector distance = GetOwner()->GetActorLocation() - _beginPosition;
	if (distance.X < DestroyDistance || distance.Y < DestroyDistance || distance.Z < DestroyDistance)
	{
		_canBeDestroyed = true;
	}

	if (_canBeDestroyed)
	{
		GetOwner()->Destroy();
	}
}

