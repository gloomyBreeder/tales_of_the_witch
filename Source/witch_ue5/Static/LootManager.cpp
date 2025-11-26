#include "LootManager.h"

void ULootManager::AddLoot(int32 amount)
{
    _soundManager->PlaySound(EBasicSoundType::Collect);
    _lootCount += amount;
    OnLootCountChanged.Broadcast(_lootCount);
}

void ULootManager::SetData(ASoundManager* soundManager)
{
    _soundManager = soundManager;
}

void ULootManager::Reset()
{
    _lootCount = 0;
    OnLootCountChanged.Broadcast(_lootCount);
}

void ULootManager::AddDestroy(int32 count)
{
    _destroyCount += count;
}

bool ULootManager::CanSpawnHealItem()
{
    if (_destroyCount >= _destroyCountForHeal)
    {
        _destroyCount = 0;

        // Generate a random number between 0 and 1 to decide if the potion spawns
        if (FMath::FRand() <= _chanceForHeal)
        {
            return true;
        }
    }

    return false;
}