#include "ActorComponents/LootAnimationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Static/GameStateManager.h"
#include "GameFramework/Character.h"

ULootAnimationComponent::ULootAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULootAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	_playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void ULootAnimationComponent::OnTimerEnd()
{
	OnLootDestroyDelegate.Broadcast();
}

void ULootAnimationComponent::AddLootItem(AActor* lootItem)
{
	float randomAnimDuration = FMath::FRandRange(AnimDurationMin, AnimDurationMax);
	FLootItemState lootState(lootItem, randomAnimDuration);
	_lootItems.Add(lootState);
	_spawnedCount++;
}

void ULootAnimationComponent::SetAnimate(bool isOn)
{
	_isAnimating = isOn;
}

void ULootAnimationComponent::SetOverlap(bool isOn)
{
	_isOverlapping = isOn;
}

float ULootAnimationComponent::EaseInCirc(float t)
{
	return 1.0f - FMath::Sqrt(1.0f - FMath::Square(t));
}

FVector ULootAnimationComponent::EaseBetween(const FVector& start, const FVector& end, float alpha)
{
	alpha = FMath::Clamp(alpha, 0.0f, 1.0f);
	float easedAlpha = EaseInCirc(alpha);
	return FMath::Lerp(start, end, easedAlpha);
}

// Called every frame
void ULootAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_isAnimating)
	{
		UpdatePosition(DeltaTime);
	}

	if (_gatheredCount == _spawnedCount && _gatheredCount > 0 && _spawnedCount > 0 && !_isTimerActivated)
	{
		_isTimerActivated = true;
		GetWorld()->GetTimerManager().SetTimer(_timerHandle, this, &ULootAnimationComponent::OnTimerEnd, 10.f, false);
	}
}

void ULootAnimationComponent::UpdatePosition(float deltaTime)
{
	if (_lootItems.Num() == 0)
	{
		return;
	}

	if (!_playerCharacter)
	{
		return;
	}

	for (FLootItemState& lootState : _lootItems)
	{
		if (!lootState.LootItem)
		{
			continue;
		}

		UStaticMeshComponent* lootMesh = lootState.LootItem->FindComponentByClass<UStaticMeshComponent>();
		if (!lootMesh)
		{
			continue;
		}

		if (!lootState.WaitedForAnimation)
		{
			float randomX = FMath::FRandRange(ImpulseMin, ImpulseMax);
			float randomY = FMath::FRandRange(ImpulseMin, ImpulseMax);
			FVector velocity = FVector(randomX, randomY, 1.0f) * ImpulseStrength;

			lootMesh->AddImpulse(velocity);
			lootState.WaitedForAnimation = true;
			continue;
		}

		if (lootMesh->IsSimulatingPhysics())
		{
			continue;
		}

		FRotator currentRotation = lootMesh->GetComponentRotation();
		float randomRotationSpeed = FMath::FRand() * RotationSpeed * deltaTime;
		currentRotation.Yaw += randomRotationSpeed;
		lootMesh->SetWorldRotation(currentRotation);

		FVector playerPosition = _playerCharacter->GetActorLocation();

		//for not gathering if too far away
		float maxDistanceFromPlayer = FVector::Dist(lootMesh->GetComponentLocation(), playerPosition);
		if (maxDistanceFromPlayer > PlayerMaxDistanceOffset && !_isOverlapping)
		{
			// zero alpha to play animation once again
			bool needUpdate = false;
			if (lootState.Alpha > 0.0f)
			{
				lootState.Alpha = 0.0f;
			}
			if (lootState.StartPosition != FVector::ZeroVector)
			{
				lootState.StartPosition = FVector::ZeroVector;
				needUpdate = true;
			}
			if (needUpdate)
			{
				lootMesh->SetSimulatePhysics(true);
			}
			continue;
		}

		if (lootState.StartPosition == FVector::ZeroVector)
		{
			lootState.StartPosition = lootMesh->GetComponentLocation();
		}

		if (lootState.Alpha < 1.0f)
		{
			lootState.Alpha += deltaTime / lootState.AnimDuration;
			FVector currentLocation = EaseBetween(lootState.StartPosition, playerPosition, lootState.Alpha);
			lootMesh->SetWorldLocation(currentLocation, false);
			
			float distanceToPlayer = FVector::Dist(lootMesh->GetComponentLocation(), playerPosition);
			if (distanceToPlayer < PlayerOffset)
			{
				OnLootItemCollect.Broadcast(lootState.LootItem);
				_gatheredCount++;
				lootState.LootItem->Destroy();
				lootState.LootItem = nullptr;
			}

			if (lootState.LootItem == nullptr)
			{
				continue;
			}
		}
	}

	if (_lootItems.Num() == 0)
	{
		_isAnimating = false;
	}
	else
	{
		// clear already destriyed elements
		_lootItems.RemoveAll([](const FLootItemState& lootState) {
			return lootState.LootItem == nullptr;
			});
	}
}
