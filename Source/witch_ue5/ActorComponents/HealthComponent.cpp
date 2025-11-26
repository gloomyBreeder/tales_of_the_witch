#include "ActorComponents/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	_health = MaxHealth;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UHealthComponent::GetHealth()
{
	return _health;
}

void UHealthComponent::Add(float amount)
{
	_health += amount;
	if (_health > MaxHealth)
	{
		_health = MaxHealth;
	}
}

void UHealthComponent::Remove(float amount)
{
	if (_health > 0)
	{
		_health = FMath::Max(0.0f, _health - amount);
	}
}

void UHealthComponent::Restore()
{
	_health = MaxHealth;
}

