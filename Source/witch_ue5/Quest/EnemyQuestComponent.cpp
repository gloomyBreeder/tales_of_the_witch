#include "EnemyQuestComponent.h"

UEnemyQuestComponent::UEnemyQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FName UEnemyQuestComponent::GetQuestId()
{
	return QuestName;
}

void UEnemyQuestComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemyQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

