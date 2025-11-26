#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LootAnimationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLootItemCollectDelegate, AActor*, LootItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLootOnDestroyDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCH_UE5_API ULootAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULootAnimationComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void AddLootItem(AActor* lootItem);
	void SetAnimate(bool isOn);
	void SetOverlap(bool isOn);

	UPROPERTY(BlueprintAssignable, Category = "Loot Animation")
	FLootItemCollectDelegate OnLootItemCollect;
	UPROPERTY(BlueprintAssignable, Category = "Loot Animation")
	FLootOnDestroyDelegate OnLootDestroyDelegate;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Animation")
	float PlayerOffset = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Animation")
	float PlayerMaxDistanceOffset = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Animation")
	float RotationSpeed = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Animation")
	float AnimDurationMin = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Animation")
	float AnimDurationMax = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Animation")
	float ImpulseStrength = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Animation")
	float ImpulseMin = -0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Animation")
	float ImpulseMax = 0.2f;

private:
	void UpdatePosition(float deltaTime);
	void OnTimerEnd();
	FVector EaseBetween(const FVector& start, const FVector& end, float alpha);
	float EaseInCirc(float t);

	//todo try make pointers
	TArray<FLootItemState> _lootItems;
	bool _isOverlapping = false;
	bool _isAnimating = false;
	bool _isTimerActivated = false;
	ACharacter* _playerCharacter;
	int32 _gatheredCount = 0;
	int32 _spawnedCount = 0;
	FTimerHandle _timerHandle;
};

USTRUCT(BlueprintType)
struct FLootItemState
{
	GENERATED_USTRUCT_BODY()

	AActor* LootItem;
	float Alpha;
	bool WaitedForAnimation;
	FVector StartPosition;
	float AnimDuration;

	FLootItemState() : LootItem(nullptr), Alpha(0.0f), WaitedForAnimation(false), StartPosition(FVector::ZeroVector), AnimDuration(1.f)
	{
	}

	FLootItemState(AActor* Item, float AnimDuration) : LootItem(Item), Alpha(0.0f), WaitedForAnimation(false), StartPosition(FVector::ZeroVector), AnimDuration(AnimDuration)
	{
	}

	bool operator==(const FLootItemState& Other) const
	{
		return this->LootItem == Other.LootItem;
	}
};