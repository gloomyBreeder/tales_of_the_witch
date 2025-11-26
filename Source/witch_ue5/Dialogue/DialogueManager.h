#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest/QuestsData.h"
#include "Static/CutSceneManager.h"
#include "Sound/SoundManager.h"
#include "DialogueManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEndDialogue, FName, DialogId);

UCLASS()
class WITCH_UE5_API ADialogueManager : public AActor
{
	GENERATED_BODY()

public:
	ADialogueManager();

	void SetData(UCutSceneManager* cutSceneManager, ASoundManager* soundManager);
	void ShowDialogue(FName DialogId);
	void ShowEndDialogue();
	FName GetEndDialogue();

	UPROPERTY(BlueprintAssignable)
	FEndDialogue OnEndDialogue;
	UFUNCTION()
	void OnCutsceneEnd(FName CutScene);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EndDialogId;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OpenDialogue(const FText& Text);
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void CloseDialogue();
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	bool IsTyping();

private:
	void Clear();

	FDialogueData* _currentDialogue = nullptr;
	FDialogueStatus* _currentDialogueStatus = nullptr;
	UCutSceneManager* _cutSceneManager;
	ASoundManager* _soundManager;
	TMap<FName, FDialogueData*> _dialogues;
	TMap<FName, FDialogueStatus*> _dialogueStatuses;

	UPROPERTY()
	UDataTable* _dataTable;
};
