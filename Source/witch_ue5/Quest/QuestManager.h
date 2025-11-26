#pragma once

#include "CoreMinimal.h"
#include "QuestsData.h"
#include "Dialogue/DialogueManager.h"
#include "Actors/EnemyCombatDirector.h"
#include "Info/InfoWindowManager.h"
#include "QuestManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestEnd, FName, QuestId);

UCLASS()
class WITCH_UE5_API UQuestManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FQuestEnd OnQuestEnd;

	void SetData(ADialogueManager* dialogueManager, AInfoWindowManager* infoManager, AEnemyCombatDirector* combatDirector, UWorld* world);
	TSharedPtr<FQuestStatus> GetQuestStatus(FName questId);
	void ShowQuestDialogue(FName questId);
	void StartQuest(FName questId);
	TSharedPtr<FQuestData> GetQuestData(FName questId);
	void TryCompleteQuest(FName questId);
	void CompleteQuest(TSharedPtr<FQuestStatus> questStatus);
	bool IsCompletedQuest(FName questId);
	void ResetAll();

protected:
	UFUNCTION()
	void OnKillEnemy(AActor* enemy);

private:
	template<typename T>
	void LoadQuestData(EQuestType questType, const FString& dataTablePath);

	template<typename T>
	T* GetQuestByType(FName questId, EQuestType questType);

	UDataTable* GetTableByType(EQuestType questType) const;

	class AGameStateManager* _gameState;

	ADialogueManager* _dialogueManager;
	AInfoWindowManager* _infoManager;
	AEnemyCombatDirector* _combatDirector;
	TMap<FName, TSharedPtr<FQuestStatus>> _questStatuses;
	UWorld* _world;

	UPROPERTY()
	TMap<EQuestType, UDataTable*> _questDataTables;
};
