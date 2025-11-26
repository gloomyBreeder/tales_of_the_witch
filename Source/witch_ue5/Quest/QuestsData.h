
#pragma once

#include "Combat/CombatStructs.h"
#include "QuestsData.generated.h"

UENUM(BlueprintType)
enum class EDialogueCharacter : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Cat = 1 UMETA(DisplayName = "Cat"),
	Player = 2 UMETA(DisplayName = "Player")
};

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	JustChat = 1 UMETA(DisplayName = "JustChat"),
	KillEnemy = 2 UMETA(DisplayName = "KillEnemy")
};

UENUM(BlueprintType)
enum class EQuestSubType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	GetTelekinesis = 1 UMETA(DisplayName = "GetTelekinesis"),
};

USTRUCT(BlueprintType)
struct FDialogueData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DialogueId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogueCharacter Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> Messages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DialogueEndCutScene = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool NeedCutSceneCamera = true;
};

USTRUCT(BlueprintType)
struct FQuestData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName QuestId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDialogueCharacter Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DialogueId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestType QuestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestSubType QuestSubType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText QuestTextNotify = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BlockMovementOnEnter = true;
};

USTRUCT(BlueprintType)
struct FQuest : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuestData QuestData;
};

USTRUCT(BlueprintType)
struct FQuestKillEnemies : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuestData QuestData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 KillCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemySpawnSettings SpawnSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
};

USTRUCT(BlueprintType)
struct FDialogueStatus
{
	GENERATED_USTRUCT_BODY()

	FName DialogueId;
	bool IsCompleted;
	int32 CurrentMessageCount;

	FDialogueStatus() : DialogueId(""), IsCompleted(false), CurrentMessageCount(-1)
	{
	}

	FDialogueStatus(FName DialogueId, bool IsCompleted, int32 CurrentMessageCount) : DialogueId(DialogueId), IsCompleted(IsCompleted), CurrentMessageCount(CurrentMessageCount)
	{
	}
};

class FQuestStatus
{
public:
	FName QuestId;
	EQuestType QuestType;
	EQuestSubType QuestSubType;
	bool IsCompleted;
	bool IsActive;

	FQuestStatus()
		: QuestId(""), QuestType(EQuestType::None), QuestSubType(EQuestSubType::None), IsCompleted(false), IsActive(false) {
	}

	FQuestStatus(FName InQuestId, EQuestType InQuestType, EQuestSubType InQuestSubType, bool completed = false, bool active = false)
		: QuestId(InQuestId), QuestType(InQuestType), QuestSubType(InQuestSubType), IsCompleted(completed), IsActive(active) {
	}

	virtual ~FQuestStatus() = default;
};

class FEnemyQuestStatus : public FQuestStatus
{
public:
	int32 KilledCount;

	FEnemyQuestStatus()
		: FQuestStatus(), KilledCount(0) {
	}

	FEnemyQuestStatus(FName InQuestId, EQuestType InQuestType, EQuestSubType InQuestSubType, bool completed = false, bool active = false, int32 InKilledCount = 0)
		: FQuestStatus(InQuestId, InQuestType, InQuestSubType, completed, active), KilledCount(InKilledCount) {
	}
};