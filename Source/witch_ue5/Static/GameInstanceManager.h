#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceManager.generated.h"

UCLASS()
class WITCH_UE5_API UGameInstanceManager : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	void ShowMainMenu();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading")
	FName StartMap = "StartMap";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

private:
	UFUNCTION()
	void OnMapLoaded(UWorld* LoadedWorld);
	
};
