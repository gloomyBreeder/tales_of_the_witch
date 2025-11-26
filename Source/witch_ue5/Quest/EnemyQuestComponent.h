#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyQuestComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCH_UE5_API UEnemyQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyQuestComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FName GetQuestId();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName QuestName;

};
