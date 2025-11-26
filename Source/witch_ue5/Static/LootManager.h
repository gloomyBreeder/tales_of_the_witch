#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundManager.h"
#include "LootManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLootCountChanged, int32, NewLootCount);

UCLASS()
class WITCH_UE5_API ULootManager : public UObject
{
    GENERATED_BODY()

public:
    void AddLoot(int32 amount);
    void SetData(ASoundManager* soundManager);
    void Reset();
    void AddDestroy(int32 count);
    bool CanSpawnHealItem();

    UPROPERTY(BlueprintAssignable)
    FLootCountChanged OnLootCountChanged;

private:
    ASoundManager* _soundManager;
    int32 _lootCount;
    int32 _destroyCount = 0;
    int32 _destroyCountForHeal = 10;
    float _chanceForHeal = 0.7f;
};