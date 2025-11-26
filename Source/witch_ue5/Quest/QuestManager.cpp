#include "QuestManager.h"
#include "InputPlayer/InputCharacter.h"
#include "EnemyQuestComponent.h"
#include "Static/GameStateManager.h"

void UQuestManager::SetData(ADialogueManager* dialogueManager, AInfoWindowManager* infoManager, AEnemyCombatDirector* combatDirector, 
	UWorld* world)
{
	_world = world;

	LoadQuestData<FQuest>(EQuestType::JustChat, TEXT("/Game/Config/DT_Quests.DT_Quests"));
	LoadQuestData<FQuestKillEnemies>(EQuestType::KillEnemy, TEXT("/Game/Config/DT_KillQuests.DT_KillQuests"));

	_dialogueManager = dialogueManager;
	_infoManager = infoManager;
	_combatDirector = combatDirector;
	_gameState = Cast<AGameStateManager>(_world->GetGameState());

	//todo maybe add in starting quest and remove after complete quest but should check if any kill quest is active
	_combatDirector->OnEnemyKill.AddUniqueDynamic(this, &UQuestManager::OnKillEnemy);
}

template<typename T>
void UQuestManager::LoadQuestData(EQuestType questType, const FString& dataTablePath)
{
	static_assert(std::is_base_of<FTableRowBase, T>::value, "T must derive from FTableRowBase");

	UDataTable* dataTable = LoadObject<UDataTable>(nullptr, *dataTablePath);
	if (dataTable)
	{
		_questDataTables.Add(questType, dataTable);

		TArray<T*> rows;
		dataTable->GetAllRows<T>("", rows);

		for (T* quest : rows)
		{
			TSharedPtr<FQuestStatus> newQuestStatus;
			if constexpr (std::is_same<T, FQuestKillEnemies>::value)
			{
				newQuestStatus = MakeShared<FEnemyQuestStatus>(
					quest->QuestData.QuestId, quest->QuestData.QuestType, quest->QuestData.QuestSubType);
			}
			else
			{
				newQuestStatus = MakeShared<FQuestStatus>(
					quest->QuestData.QuestId, quest->QuestData.QuestType, quest->QuestData.QuestSubType);
			}

			if (newQuestStatus)
			{
				_questStatuses.Add(newQuestStatus->QuestId, newQuestStatus);
			}
		}
	}
}

template<typename T>
T* UQuestManager::GetQuestByType(FName questId, EQuestType questType)
{
	static_assert(std::is_base_of<FTableRowBase, T>::value, "T must derive from FTableRowBase");

	const UDataTable* table = GetTableByType(questType);
	if (!table)
	{
		return nullptr;
	}

	TArray<T*> rows;
	table->GetAllRows<T>("", rows);

	for (T* quest : rows)
	{
		if (questId == quest->QuestData.QuestId)
		{
			return quest;
		}
	}

	return nullptr;
}

void UQuestManager::OnKillEnemy(AActor* enemy)
{
	UEnemyQuestComponent* questComponent = enemy->FindComponentByClass<UEnemyQuestComponent>();
	if (questComponent)
	{
		TSharedPtr<FQuestStatus> questStatus = GetQuestStatus(questComponent->GetQuestId());

		if (!questStatus || questStatus->IsCompleted || !questStatus->IsActive)
		{
			return;
		}

		if (TSharedPtr<FEnemyQuestStatus> enemyQuestStatus = StaticCastSharedPtr<FEnemyQuestStatus>(questStatus))
		{
			if (FQuestKillEnemies* enemyQuest = GetQuestByType<FQuestKillEnemies>(questComponent->GetQuestId(), EQuestType::KillEnemy))
			{
				int32& killCount = enemyQuestStatus->KilledCount;
				killCount++;

				if (killCount == enemyQuest->KillCount)
				{
					CompleteQuest(questStatus);
				}
			}
		}
	}
}

TSharedPtr<FQuestStatus> UQuestManager::GetQuestStatus(FName questId)
{
	if (_questStatuses.Contains(questId))
	{
		return _questStatuses[questId];
	}

	return nullptr;
}

void UQuestManager::ShowQuestDialogue(FName questId)
{
	TSharedPtr<FQuestData> questData = GetQuestData(questId);
	_dialogueManager->ShowDialogue(questData->DialogueId);
}

void UQuestManager::StartQuest(FName questId)
{
	TSharedPtr<FQuestStatus> status = GetQuestStatus(questId);

	if (!status || status->IsCompleted || status->IsActive)
	{
		return;
	}

	bool& isActive = status->IsActive;
	isActive = true;
	ShowQuestDialogue(questId);
}

TSharedPtr<FQuestData> UQuestManager::GetQuestData(FName questId)
{
	TSharedPtr<FQuestStatus> questStatus = GetQuestStatus(questId);
	if (!questStatus)
	{
		return nullptr;
	}

	if (questStatus->QuestType == EQuestType::KillEnemy)
	{
		if (FQuestKillEnemies* enemyQuest = GetQuestByType<FQuestKillEnemies>(questId, EQuestType::KillEnemy))
		{
			return MakeShared<FQuestData>(enemyQuest->QuestData);
		}
	}
	else if (questStatus->QuestType == EQuestType::JustChat)
	{
		if (FQuest* basicQuest = GetQuestByType<FQuest>(questId, EQuestType::JustChat))
		{
			return MakeShared<FQuestData>(basicQuest->QuestData);
		}
	}

	return nullptr;
}

void UQuestManager::TryCompleteQuest(FName questId)
{
	TSharedPtr<FQuestStatus> questStatus = GetQuestStatus(questId);
	if (questStatus)
	{
		if (questStatus->IsCompleted || !questStatus->IsActive)
		{
			return;
		}

		if (questStatus->QuestType == EQuestType::JustChat)
		{
			CompleteQuest(questStatus);
		}
		else if (questStatus->QuestType == EQuestType::KillEnemy)
		{
			if (FQuestKillEnemies* enemyQuest = GetQuestByType<FQuestKillEnemies>(questId, EQuestType::KillEnemy))
			{
				//todo maybe relocate in another class idk
				_combatDirector->SpawnEnemies(enemyQuest->Location, enemyQuest->SpawnSettings);
			}
		}
	}
}

void UQuestManager::CompleteQuest(TSharedPtr<FQuestStatus> questStatus)
{
	bool& isCompleted = questStatus->IsCompleted;
	bool& isActive = questStatus->IsActive;
	if (isCompleted || !isActive)
	{
		return;
	}

	isCompleted = true;
	isActive = false;
	OnQuestEnd.Broadcast(questStatus->QuestId);

	if (questStatus->QuestSubType == EQuestSubType::GetTelekinesis)
	{
		ACharacter* player = UGameplayStatics::GetPlayerCharacter(_world, 0);
		if (player)
		{
			AInputCharacter* character = Cast<AInputCharacter>(player);
			if (character)
			{
				character->UnlockTelekinesis();
			}
		}
	}

	TSharedPtr<FQuestData> quest = GetQuestData(questStatus->QuestId);
	if (quest && !quest->QuestTextNotify.IsEmpty())
	{
		_infoManager->ShowNotify(quest->QuestTextNotify);
		_gameState->GetSoundManager()->PlaySound(EBasicSoundType::Notify);
	}
}

bool UQuestManager::IsCompletedQuest(FName questId)
{
	TSharedPtr<FQuestStatus> quest = GetQuestStatus(questId);
	if (quest)
	{
		return quest->IsCompleted;
	}

	// perestrahovOCHKA
	return true;
}

UDataTable* UQuestManager::GetTableByType(EQuestType questType) const
{
	if (_questDataTables.Contains(questType))
	{
		return _questDataTables[questType];
	}

	return nullptr;
}

void UQuestManager::ResetAll()
{
	_questStatuses.Empty();
}
