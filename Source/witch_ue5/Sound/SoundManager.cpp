#include "Sound/SoundManager.h"
#include "Components/AudioComponent.h"
#include "Static/UtilityHelper.h"
#include "Kismet/GameplayStatics.h"

ASoundManager::ASoundManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASoundManager::BeginPlay()
{
	Super::BeginPlay();
}

void ASoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoundManager::Init()
{
	_soundDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Config/DT_Sounds.DT_Sounds"));
}

void ASoundManager::PlaySound(EPlayerSoundType playerSoundType, FVector location, bool loop)
{
	FName soundName = GetSoundName(UtilityHelper::EnumToFName(playerSoundType));
	USoundBase* sound = GetSoundByName(soundName);

	if (sound)
	{
		PlaySound(soundName, sound, location, loop);
	}
}

void ASoundManager::PlaySound(EEnemySoundType enemySoundType, FVector location, bool loop)
{
	FName soundName = GetSoundName(UtilityHelper::EnumToFName(enemySoundType));
	USoundBase* sound = GetSoundByName(soundName);

	if (sound)
	{
		PlaySound(soundName, sound, location, loop);
	}
}

void ASoundManager::PlaySound(EBasicSoundType basicSoundType, FVector location, bool loop)
{
	FName soundName = GetSoundName(UtilityHelper::EnumToFName(basicSoundType));
	USoundBase* sound = GetSoundByName(soundName);

	if (sound)
	{
		PlaySound(soundName, sound, location, loop);
	}
}

void ASoundManager::PlaySound(ElementType elementType, FVector location, bool loop)
{
	FName soundName = GetSoundName(UtilityHelper::EnumToFName(elementType));
	USoundBase* sound = GetSoundByName(soundName);

	if (sound)
	{
		PlaySound(soundName, sound, location, loop);
	}
}

USoundBase* ASoundManager::GetSoundByName(FName soundName)
{
	//FSoundData* soundRow = _soundDataTable->FindRow<FSoundData>(soundName, TEXT(""));
	if (!_soundDataTable || !_soundDataTable->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Sound DataTable is NULL!"));
		return nullptr;
	}
	check(_soundDataTable);
	check(_soundDataTable->RowStruct);
	FSoundData* soundRow = _soundDataTable->FindRow<FSoundData>(soundName, TEXT(""));
	if (!soundRow)
	{
		UE_LOG(LogTemp, Error, TEXT("Sound not found in DataTable: %s"), *soundName.ToString());
		return nullptr;
	}

	if (!soundRow->Sound)
	{
		UE_LOG(LogTemp, Error, TEXT("Sound asset is NULL for: %s"), *soundName.ToString());
		return nullptr;
	}

	return soundRow ? soundRow->Sound : nullptr;
}

FName ASoundManager::GetSoundName(FName soundName)
{
	if (_soundDataTable)
	{
		FString soundString = soundName.ToString();
		int32 colonIndex;
		if (soundString.FindChar(':', colonIndex))
		{
			// remove enum part with ::
			return FName(soundString.RightChop(colonIndex + 2));
		}
	}

	return FName("");
}

void ASoundManager::PlaySound(FName soundName, USoundBase* sound, FVector location, bool loop)
{
	if (loop)
	{
		if (!_zaloopedSounds.Contains(soundName))
		{
			UAudioComponent* audioComponent = UGameplayStatics::SpawnSoundAtLocation(this, sound, location);
			if (audioComponent)
			{
				audioComponent->Play();
			}
			_zaloopedSounds.Add(soundName, audioComponent);
		}
	}
	else
	{
		if (location == FVector::Zero())
		{
			UGameplayStatics::PlaySound2D(this, sound);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(this, sound, location);
		}
	}
}

void ASoundManager::StopSound(FName soundName)
{
	FName sound = GetSoundName(soundName);
	if (_zaloopedSounds.Contains(sound))
	{
		UAudioComponent* audioComponent = _zaloopedSounds[sound];
		audioComponent->Stop();
		_zaloopedSounds.Remove(sound);
	}
}