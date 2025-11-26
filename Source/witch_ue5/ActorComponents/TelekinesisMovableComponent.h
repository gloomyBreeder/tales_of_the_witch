#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TelekinesisMovableComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCH_UE5_API UTelekinesisMovableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTelekinesisMovableComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Destroy")
	float DestroyDistance = -30000.0f;

private:
	FVector _beginPosition;
	bool _canBeDestroyed;
		
};
