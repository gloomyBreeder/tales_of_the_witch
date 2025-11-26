#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Combat/CombatStructs.h"
#include "SoundManager.generated.h"

UCLASS()
class WITCH_UE5_API ASoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASoundManager();
	virtual void Tick(float DeltaTime) override;

	void Init();

	void PlaySound(EPlayerSoundType playerSoundType, FVector location = FVector::Zero(), bool loop = false);
	void PlaySound(EEnemySoundType enemySoundType, FVector location = FVector::Zero(), bool loop = false);
	void PlaySound(EBasicSoundType basicSoundType, FVector location = FVector::Zero(), bool loop = false);
	void PlaySound(ElementType elementType, FVector location = FVector::Zero(), bool loop = false);
	void PlaySound(FName soundName, USoundBase* sound, FVector location = FVector::Zero(), bool loop = false);
	void StopSound(FName soundName);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UDataTable* _soundDataTable;

	USoundBase* GetSoundByName(FName soundName);
	FName GetSoundName(FName soundName);
	TMap<FName, UAudioComponent*> _zaloopedSounds;
};

USTRUCT(BlueprintType)
struct FSoundData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Sound;
};

UENUM(BlueprintType)
enum class EPlayerSoundType : uint8
{
	None UMETA(DisplayName = "None"),
	FootStep UMETA(DisplayName = "FootStep"),
	Jump UMETA(DisplayName = "Jump"),
	DoubleJump UMETA(DisplayName = "DoubleJump"),
	Dash UMETA(DisplayName = "Dash"),
	Telekinesis UMETA(DisplayName = "Telekinesis"),
	Land UMETA(DisplayName = "Land"),
	GroundLand UMETA(DisplayName = "GroundLand"),
	DashTimer UMETA(DisplayName = "DashTimer"),
	Equip UMETA(DisplayName = "Equip")
};

UENUM(BlueprintType)
enum class EEnemySoundType : uint8
{
	None UMETA(DisplayName = "None"),
	EnemyStep UMETA(DisplayName = "Step")
};

UENUM(BlueprintType)
enum class EBasicSoundType : uint8
{
	None UMETA(DisplayName = "None"),
	Notify UMETA(DisplayName = "Notify"),
	Collect UMETA(DisplayName = "Collect"),
	Teleport UMETA(DisplayName = "Teleport"),
	HitProjectile UMETA(DisplayName = "HitProjectile"),
	LootSpawn UMETA(DisplayName = "LootSpawn"),
	BirdFly UMETA(DisplayName = "BirdFly"),
	GetHeal UMETA(DisplayName = "GetHeal")
};