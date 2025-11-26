#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class WITCH_UE5_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interact")
	void OnEnterInteractionZone(AActor* interactableActor, bool disableMovement, bool changeCamera);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	void OnExitInteractionZone(AActor* interactableActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
	void InteractRequest(AActor* interactableActor);
};
