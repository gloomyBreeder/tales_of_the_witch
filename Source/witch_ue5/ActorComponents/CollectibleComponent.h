#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CollectibleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCH_UE5_API UCollectibleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCollectibleComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
		
};
