#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InfoWindowManager.generated.h"

UCLASS()
class WITCH_UE5_API AInfoWindowManager : public AActor
{
	GENERATED_BODY()
	
public:
	AInfoWindowManager();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowNotify(const FText& Text);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowInfoWindow(const FText& Text);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
