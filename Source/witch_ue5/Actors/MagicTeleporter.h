#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Static/GameStateManager.h"
#include "MagicTeleporter.generated.h"

UCLASS()
class WITCH_UE5_API AMagicTeleporter : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagicTeleporter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void EnterTeleporter(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult);
	UFUNCTION()
	virtual void ExitTeleporter(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int32 otherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "Teleporter")
	AMagicTeleporter* TargetTeleporter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleporter")
	UBoxComponent* TriggerBox;
	UPROPERTY()
	bool IsTeleporting;
	UPROPERTY()
	bool IsAvailable;

};
