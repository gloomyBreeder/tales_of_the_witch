#include "GameStateManager.h"
#include "GameFramework/Actor.h"
#include "Dialogue/DialogueTrigger.h"
#include "Static/GameInstanceManager.h"
#include "Characters/CatCharacter.h"

#include "EngineUtils.h"

AGameStateManager::AGameStateManager()
{
}

void AGameStateManager::BeginPlay()
{
    Super::BeginPlay();

#if WITH_EDITOR
    if (GIsEditor && GetWorld() && GetWorld()->WorldType == EWorldType::PIE)
    {
        UGameInstanceManager* gameManager = Cast<UGameInstanceManager>(GetGameInstance());
        if (gameManager)
        {
            gameManager->ShowMainMenu();
        }
    }
#endif
    
}

void AGameStateManager::SetCanEndGame()
{
    _canEndGame = true;
}

bool AGameStateManager::CanEndGame()
{
    return _canEndGame;
}

void AGameStateManager::OnEndDialogue(FName Dialogue)
{
    //not very good decision but here we are
    if (_currentStartDialogueIndex < _startDialogueList.Num())
    {
        for (ADialogueTrigger* dialogueTrigger : _startDialogueList)
        {
            if (dialogueTrigger->GetDialogueId() == Dialogue)
            {
                OnEndDialogueCallback();
                _currentStartDialogueIndex++;
                break;
            }
        }
    }
    else if (Dialogue == _dialogueManager->GetEndDialogue())
    {
        UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Type::Quit, false);
    }
}

void AGameStateManager::OnEndCutScene(FName CutScene)
{
    if (CutScene == BeginCutScene)
    {
        APlayerController* playerController = GetWorld()->GetFirstPlayerController();
        playerController->SetViewTarget(_player);
        _player->SetAnimationControl(true);
    }
}

void AGameStateManager::OnEndDialogueCallback()
{
    if (_currentStartDialogueIndex == 0)
    {
        _player->EnableOnlyMovement(true);
    }
    else
    {
        _player->ShowUI();
        _player->SetWeaponState(true);
        _player->EnableAll(true);
        _player->SavePosition();
        _soundManager->PlaySound(EPlayerSoundType::Equip, _player->GetActorLocation());
        GetWorldTimerManager().SetTimer(_startTimer, this, &AGameStateManager::OnEndAllDialogue, 0.5f, false);
        PlayGameMusic();
    }
}

void AGameStateManager::LoadAllManagers()
{
    if (!_cutSceneManager)
    {
        _cutSceneManager = NewObject<UCutSceneManager>();
        _cutSceneManager->Init(GetWorld());
        _cutSceneManager->AddToRoot();
        _cutSceneManager->OnCutsceneFinished.AddUniqueDynamic(this, &AGameStateManager::OnEndCutScene);
    }

    if (SoundManagerActor)
    {
        FActorSpawnParameters spawnParams;
        AActor* soundManager = GetWorld()->SpawnActor<AActor>(SoundManagerActor, K2_GetActorLocation(), FRotator::ZeroRotator, spawnParams);
        if (soundManager)
        {
            _soundManager = Cast<ASoundManager>(soundManager);
            _soundManager->Init();
        }
    }

    if (DialogueManagerActor)
    {
        FActorSpawnParameters spawnParams;
        AActor* dialogueManager = GetWorld()->SpawnActor<AActor>(DialogueManagerActor, K2_GetActorLocation(), FRotator::ZeroRotator, spawnParams);
        if (dialogueManager)
        {
            _dialogueManager = Cast<ADialogueManager>(dialogueManager);
            _dialogueManager->SetData(_cutSceneManager, _soundManager);
            _dialogueManager->OnEndDialogue.AddUniqueDynamic(this, &AGameStateManager::OnEndDialogue);
        }
    }

    if (InfoManagerActor)
    {
        FActorSpawnParameters spawnParams;
        AActor* infoManager = GetWorld()->SpawnActor<AActor>(InfoManagerActor, K2_GetActorLocation(), FRotator::ZeroRotator, spawnParams);
        if (infoManager)
        {
            _infoManager = Cast<AInfoWindowManager>(infoManager);
        }
    }

    if (CombatDirectorActor)
    {
        FActorSpawnParameters spawnParams;
        AActor* combatDirector = GetWorld()->SpawnActor<AActor>(CombatDirectorActor, K2_GetActorLocation(), FRotator::ZeroRotator, spawnParams);
        if (combatDirector)
        {
            _combatDirector = Cast<AEnemyCombatDirector>(combatDirector);
        }
    }

    if (CrowSpawnerActor)
    {
        FActorSpawnParameters spawnParams;
        AActor* crowSpawner = GetWorld()->SpawnActor<AActor>(CrowSpawnerActor, K2_GetActorLocation(), FRotator::ZeroRotator, spawnParams);
        if (crowSpawner)
        {
            _crowSpawner = Cast<ACrowSpawner>(crowSpawner);
        }
    }

    if (!_lootManager)
    {
        _lootManager = NewObject<ULootManager>();
        _lootManager->AddToRoot();
        _lootManager->SetData(_soundManager);
        _lootManager->Reset();
    }

    if (!_questManager)
    {
        _questManager = NewObject<UQuestManager>();
        _questManager->AddToRoot();
        _questManager->ResetAll();
        _questManager->SetData(_dialogueManager, _infoManager, _combatDirector, GetWorld());
    }
}

void AGameStateManager::StartGame()
{
    LoadAllManagers();

    APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    if (!playerController)
    {
        throw std::runtime_error("no player controller");
    }

    bool foundPlayer = false;

    //not good decision but i didnt figure out how to do it better
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* actor = *It;

        if (ADialogueTrigger* dialogueTrigger = Cast<ADialogueTrigger>(actor))
        {
            if (DebugSkipStart)
            {
                dialogueTrigger->Destroy();
            }
            else
            {
                dialogueTrigger->Init();
                _startDialogueList.Add(dialogueTrigger);
            }
            continue;
        }

        if (AEnemySpawner* enemySpawner = Cast<AEnemySpawner>(actor))
        {
            _enemySpawners.Add(enemySpawner);
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "add Enemy Spawner");
            //enemySpawner->Init();
            continue;
        }

        if (ACatCharacter* catCharacter = Cast<ACatCharacter>(actor))
        {
            catCharacter->Init();
            continue;
        }

        if (foundPlayer)
        {
            continue;
        }

        if (AInputCharacter* character = Cast<AInputCharacter>(actor))
        {
            foundPlayer = true;
            playerController->Possess(character);
            _player = Cast<AInputCharacter>(playerController->GetPawn());
            _player->Init();
        }
    }

    if (!_player)
    {
        throw std::runtime_error("no player");
    }

    OnStartGame();

    //need timer because begin play doesn't play right after possess
    //GetWorldTimerManager().SetTimer(_startTimer, this, &AGameStateManager::OnStartGame, StartTimer, false);
}

void AGameStateManager::OnStartGame()
{
    PlayAmbient();

    if (DebugSkipStart)
    {
        _player->SetActorLocation(StartPlayerCoors);
        _player->ShowUI();
        _player->SetAnimationControl(true);
        _player->SavePosition();
    }
    else
    {
        _player->SetWeaponState(false);
        _player->EnableAll(false);
        _player->SetAnimationControl(false);
        _cutSceneManager->PlayLevelSequence(BeginCutScene, false, false);
    }
}

void AGameStateManager::OnEndAllDialogue()
{
    _player->ShowInfoUI();

    // fix freezing 
    GetWorldTimerManager().SetTimer(_startTimer, this, &AGameStateManager::InitNextSpawner, 0.5f, true);
}

void AGameStateManager::InitNextSpawner()
{
    if (_currentEnemySpawnerIndex >= _enemySpawners.Num())
    {
        GetWorldTimerManager().ClearTimer(_startTimer);
        return;
    }

    if (AEnemySpawner* spawner = _enemySpawners[_currentEnemySpawnerIndex])
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "spawner Init");
        spawner->Init();
    }

    ++_currentEnemySpawnerIndex;
}
