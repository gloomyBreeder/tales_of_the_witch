#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealItem.generated.h"

UCLASS()
class WITCH_UE5_API AHealItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealItem();
	virtual void Tick(float DeltaTime) override;

	void AddHeal();

protected:
	virtual void BeginPlay() override;

};
