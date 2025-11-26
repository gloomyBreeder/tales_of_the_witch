#pragma once

#include "CoreMinimal.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h"
#include "MovieSceneSequencePlayer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "CutSceneManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCutsceneFinishedDelegate, FName, CutsceneName);

UCLASS()
class WITCH_UE5_API UCutSceneManager : public UObject
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FCutsceneFinishedDelegate OnCutsceneFinished;

    UFUNCTION()
    void HandleCutsceneFinished();

    void PlayLevelSequence(FName cutsceneName, bool needCamera, bool enablePlayerAfterPlay = true);

    void Init(UWorld* world);

private:
    void EnablePlayerCamera();
    UWorld* _world;
    FName _cutsceneName = NAME_None;
    bool _enablePlayerAfterPlay;

    UPROPERTY()
    ULevelSequencePlayer* _currentSequencePlayer;
    
};
