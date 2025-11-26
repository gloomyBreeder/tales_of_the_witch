#include "DialogueManager.h"


ADialogueManager::ADialogueManager()
{
}

void ADialogueManager::SetData(UCutSceneManager* cutSceneManager, ASoundManager* soundManager)
{
	_dataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Config/DT_Dialogs.DT_Dialogs"));
	_soundManager = soundManager;
	_cutSceneManager = cutSceneManager;
	_cutSceneManager->OnCutsceneFinished.AddDynamic(this, &ADialogueManager::OnCutsceneEnd);
}

void ADialogueManager::OnCutsceneEnd(FName CutScene)
{
	if (_currentDialogue && _currentDialogue->DialogueEndCutScene == CutScene)
	{
		OnEndDialogue.Broadcast(_currentDialogue->DialogueId);
		Clear();
	}
}

void ADialogueManager::ShowDialogue(FName DialogId)
{
	if (IsTyping())
	{
		return;
	}

	if (!_dataTable)
	{
		return;
	}

	if (!_currentDialogue && !_currentDialogueStatus)
	{
		if (_dialogues.Num() > 0 && _dialogues.Contains(DialogId) && _dialogueStatuses.Num() > 0 && _dialogueStatuses.Contains(DialogId))
		{
			_currentDialogue = _dialogues[DialogId];
			_currentDialogueStatus = _dialogueStatuses[DialogId];
		}
		else
		{
			TArray<FDialogueData*> dialoguesRows;
			_dataTable->GetAllRows<FDialogueData>("", dialoguesRows);

			for (FDialogueData* dialogueData : dialoguesRows)
			{
				if (dialogueData->DialogueId == DialogId)
				{
					_currentDialogue = dialogueData;
					_currentDialogueStatus = new FDialogueStatus(DialogId, false, -1);
					_dialogues.Add(DialogId, _currentDialogue);
					_dialogueStatuses.Add(DialogId, _currentDialogueStatus);
					break;
				}
			}
		}
	}

	if (_currentDialogue && _currentDialogueStatus)
	{
		if (_currentDialogueStatus->IsCompleted)
		{
			return;
		}

		if (_currentDialogue->Messages.Num() == 0)
		{
			return;
		}

		int32& currentMessage = _currentDialogueStatus->CurrentMessageCount;

		if (currentMessage + 1 < _currentDialogue->Messages.Num())
		{
			currentMessage++;

			//play sound on first message
			if (currentMessage == 0 && _currentDialogue->Sound != nullptr)
			{
				_soundManager->PlaySound(FName("DialogueSound"), _currentDialogue->Sound);
			}

			OpenDialogue(_currentDialogue->Messages[currentMessage]);
		}
		else
		{
			bool& isCompleted = _currentDialogueStatus->IsCompleted;
			isCompleted = true;
			CloseDialogue();

			//check if need to play cut scene
			if (!_currentDialogue->DialogueEndCutScene.IsNone())
			{
				_cutSceneManager->PlayLevelSequence(_currentDialogue->DialogueEndCutScene, _currentDialogue->NeedCutSceneCamera);
			}
			else
			{
				OnEndDialogue.Broadcast(DialogId);
				Clear();
			}
		}
	}
}

void ADialogueManager::ShowEndDialogue()
{
	ShowDialogue(EndDialogId);
}

FName ADialogueManager::GetEndDialogue()
{
	return EndDialogId;
}

void ADialogueManager::Clear()
{
	_currentDialogue = nullptr;
	_currentDialogueStatus = nullptr;
}
