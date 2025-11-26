#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Static/LootManager.h"
#include "Info/InfoWindowManager.h"
#include "Actors/EnemyCombatDirector.h"
#include "Actors/CrowSpawner.h"
#include "Quest/QuestManager.h"
#include "Components/PostProcessComponent.h"
#include "Dialogue/DialogueManager.h"
#include "Sound/SoundManager.h"
#include "CutSceneManager.h"
#include "Actors/EnemySpawner.h"
#include "InputPlayer/InputCharacter.h"
#include "GameStateManager.generated.h"

class ADialogueTrigger;

UCLASS()
class WITCH_UE5_API AGameStateManager : public AGameStateBase
{
	GENERATED_BODY()

public:
	AGameStateManager();
	virtual void BeginPlay() override;
	void SetCanEndGame();
	bool CanEndGame();

	UFUNCTION(BlueprintCallable)
	ULootManager* GetLootManager() const { return _lootManager; }
	UFUNCTION(BlueprintCallable)
	UQuestManager* GetQuestManager() const { return _questManager; }
	UFUNCTION(BlueprintCallable)
	ADialogueManager* GetDialogueManager() const { return _dialogueManager; }
	UFUNCTION(BlueprintCallable)
	UCutSceneManager* GetCutSceneManager() const { return _cutSceneManager; }
	UFUNCTION(BlueprintCallable)
	AInfoWindowManager* GetInfoManager() const { return _infoManager; }
	UFUNCTION(BlueprintCallable)
	AEnemyCombatDirector* GetCombatDirector() const { return _combatDirector; }
	UFUNCTION(BlueprintCallable)
	ASoundManager* GetSoundManager() const { return _soundManager; }
	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintImplementableEvent, Category = "Music")
	void PlayGameMusic();
	UFUNCTION(BlueprintImplementableEvent, Category = "Music")
	void PlayAmbient();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	TSubclassOf<AActor> DialogueManagerActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	TSubclassOf<AActor> InfoManagerActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	TSubclassOf<AActor> CombatDirectorActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	TSubclassOf<AActor> SoundManagerActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	TSubclassOf<AActor> CrowSpawnerActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player")
	FVector StartPlayerCoors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	bool DebugSkipStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start")
	FName BeginCutScene;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start")
	float StartTimer = 3.f;
	
	UFUNCTION()
	void OnEndDialogue(FName Dialogue);
	UFUNCTION()
	void OnEndCutScene(FName CutScene);

private:
	void OnEndDialogueCallback();
	void LoadAllManagers();
	void OnStartGame();
	void OnEndAllDialogue();
	void InitNextSpawner();

	TArray<ADialogueTrigger*> _startDialogueList;
	ULootManager* _lootManager;
	UQuestManager* _questManager;
	ADialogueManager* _dialogueManager;
	UCutSceneManager* _cutSceneManager;
	AEnemyCombatDirector* _combatDirector;
	AInfoWindowManager* _infoManager;
	ASoundManager* _soundManager;
	ACrowSpawner* _crowSpawner;
	AInputCharacter* _player;
	FTimerHandle _startTimer;
	bool _canEndGame = false;
	int32 _currentStartDialogueIndex = 0;
	int32 _currentEnemySpawnerIndex = 0;
	TArray<AEnemySpawner*> _enemySpawners;
};