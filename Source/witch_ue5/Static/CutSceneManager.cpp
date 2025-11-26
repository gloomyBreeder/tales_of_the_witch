#include "CutSceneManager.h"
#include "InputPlayer/InputCharacter.h"

void UCutSceneManager::Init(UWorld* world)
{
    _world = world;
}

void UCutSceneManager::HandleCutsceneFinished()
{
    if (!_currentSequencePlayer)
    {
        return;
    }

    OnCutsceneFinished.Broadcast(_cutsceneName);

    _currentSequencePlayer = nullptr;
    _cutsceneName = NAME_None;

    EnablePlayerCamera();
}

void UCutSceneManager::EnablePlayerCamera()
{
    APlayerController* playerController = _world->GetFirstPlayerController();

    if (playerController)
    {
        AActor* player = playerController->GetPawn();
        if (player)
        {
            playerController->PlayerCameraManager->SetManualCameraFade(1.0f, FLinearColor::Black, false);
            playerController->SetViewTarget(nullptr);
            playerController->SetViewTarget(player);
            playerController->PlayerCameraManager->SetManualCameraFade(0.0f, FLinearColor::Black, false);

            AInputCharacter* inputCharacter = Cast<AInputCharacter>(player);
            if (_enablePlayerAfterPlay && inputCharacter)
            {
                inputCharacter->EnableAll(true);
            }
        }
    }
}

void UCutSceneManager::PlayLevelSequence(FName cutsceneName, bool needCamera, bool enablePlayerAfterPlay)
{
    FString sequencePath = FString::Printf(TEXT("/Game/CutScenes/%s.%s"), *cutsceneName.ToString(), *cutsceneName.ToString());
    ULevelSequence* levelSequence = LoadObject<ULevelSequence>(nullptr, *sequencePath);

    if (!levelSequence)
    {
        return;
    }

    FMovieSceneSequencePlaybackSettings playbackSettings;
    playbackSettings.bAutoPlay = true;

    ALevelSequenceActor* levelSequenceActor;
    _currentSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(_world, levelSequence, playbackSettings, levelSequenceActor);

    if (_currentSequencePlayer)
    {
        _cutsceneName = cutsceneName;
        _currentSequencePlayer->Play();
        _currentSequencePlayer->OnFinished.AddDynamic(this, &UCutSceneManager::HandleCutsceneFinished);
    }

    _enablePlayerAfterPlay = enablePlayerAfterPlay;

    if (!needCamera)
    {
        EnablePlayerCamera();
    }
}