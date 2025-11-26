#include "Actors/MagicTeleporter.h"
#include "GameFramework/Character.h"
#include "InputPlayer/InputCharacter.h"

AMagicTeleporter::AMagicTeleporter()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMagicTeleporter::EnterTeleporter);
	TriggerBox->OnComponentEndOverlap.AddUniqueDynamic(this, &AMagicTeleporter::ExitTeleporter);
	IsTeleporting = false;
}

void AMagicTeleporter::BeginPlay()
{
	Super::BeginPlay();
}

void AMagicTeleporter::EnterTeleporter(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, 
	int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult)
{
	if (otherActor && otherActor != this && TargetTeleporter && !IsTeleporting)
	{
		AInputCharacter* playerInputCharacter = Cast<AInputCharacter>(otherActor);

		if (playerInputCharacter && !TargetTeleporter->IsTeleporting)
		{
			IsTeleporting = true;
			playerInputCharacter->Rotate(TargetTeleporter->GetActorRotation());
			playerInputCharacter->SetActorLocation(TargetTeleporter->GetActorLocation());
			Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EBasicSoundType::Teleport, GetActorLocation());
		}
	}
}

void AMagicTeleporter::ExitTeleporter(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, 
	int32 otherBodyIndex)
{
	if (otherActor && otherActor != this)
	{
		AInputCharacter* playerInputCharacter = Cast<AInputCharacter>(otherActor);
		if (playerInputCharacter && TargetTeleporter && !IsTeleporting)
		{
			TargetTeleporter->IsTeleporting = false;
		}
	}
}

void AMagicTeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

