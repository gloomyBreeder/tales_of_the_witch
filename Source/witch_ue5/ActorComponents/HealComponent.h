#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorComponents/HealthComponent.h"
#include "ActorComponents/AnimationComponent.h"
#include "HealComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCH_UE5_API UHealComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitialHeal();
	void Heal();
	bool CanHeal();
	bool IsHealing();
	void AddHeal();
	void Restore();

	UFUNCTION(BlueprintCallable)
	int32 GetHealsNum();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealsNum = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AmountToHeal = 25.0f;

private:
	UAnimationComponent* _animationComponent;
	UHealthComponent* _healthComponent;
	int32 _healsNum;
};
