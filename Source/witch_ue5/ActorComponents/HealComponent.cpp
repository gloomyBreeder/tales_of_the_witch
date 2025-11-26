#include "ActorComponents/HealComponent.h"

UHealComponent::UHealComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealComponent::BeginPlay()
{
	Super::BeginPlay();

	_animationComponent = GetOwner()->FindComponentByClass<UAnimationComponent>();
	_healthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
	_healsNum = MaxHealsNum;
}

void UHealComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealComponent::InitialHeal()
{
	if (_animationComponent)
	{
		_animationComponent->PlayHealAnim();
	}
}

void UHealComponent::Heal()
{
	_healthComponent->Add(AmountToHeal);
	_healsNum--;
}

bool UHealComponent::CanHeal()
{
	return _healsNum > 0;
}

bool UHealComponent::IsHealing()
{
	return _animationComponent && _animationComponent->IsHealing();
}

void UHealComponent::AddHeal()
{
	_healsNum++;
}

void UHealComponent::Restore()
{
	_healsNum = MaxHealsNum;
}

int32 UHealComponent::GetHealsNum()
{
	return _healsNum;
}

