#include "ProjectileSpawnComponent.h"
#include "GameFramework/Character.h"

UProjectileSpawnComponent::UProjectileSpawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UProjectileSpawnComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateAnimInst();
	GetData();
}

void UProjectileSpawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UProjectileSpawnComponent::GetData()
{
	_parentMesh = Cast<ACharacter>(GetOwner())->GetMesh();
	_socketLocation = _parentMesh->GetSocketTransform(SpawnSocketName);
	_dataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Config/DT_ParticlesProjectiles.DT_ParticlesProjectiles"));
}

void UProjectileSpawnComponent::HandleOnMontageNotifyBegin(FName notifyName, const FBranchingPointNotifyPayload& branchingPayload)
{
	if (notifyName.ToString() == "OnShootProjectileBegin")
	{
		// if no save projectile data do nothing
		if (_element == ElementType::None && 
			_endLocation == FVector::Zero() && 
			_projectile == nullptr)
		{
			return;
		}

		if (_projectile)
		{
			_projectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			_projectile->MoveTo(GetOwner(), _endLocation, _element);
			_projectile = nullptr;

			// spawn next projectile for particle effect keeping in socket
			GetWorld()->GetTimerManager().SetTimer(_timerHandle, this, &UProjectileSpawnComponent::OnTimerEnd, DelayAfterShoot, false);
		}
	}
	else if (notifyName.ToString() == "OnParticleChange")
	{
		if (!_projectile)
		{
			// spawn particle after stop for playing animation
			SpawnProjectile();
		}
	}
}

void UProjectileSpawnComponent::SpawnProjectile()
{
	if (!_dataTable)
	{
		return;
	}

	TArray<FParticleProjectile*> allProjectileRows;
	_dataTable->GetAllRows<FParticleProjectile>("", allProjectileRows);

	TSubclassOf<class AProjectile> projectileToSpawn;
	for (FParticleProjectile* particleProjectile : allProjectileRows)
	{
		if (_element == particleProjectile->ElementType)
		{
			projectileToSpawn = particleProjectile->Projectile;
			break;
		}
	}

	if (projectileToSpawn)
	{
		FActorSpawnParameters spawnParams;

		_projectile = GetWorld()->SpawnActor<AProjectile>(projectileToSpawn.Get(), _socketLocation, spawnParams);

		if (_projectile)
		{
			if (_parentMesh)
			{
				_projectile->AttachToComponent(_parentMesh, 
					FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), 
					SpawnSocketName);
			}
			_projectile->SetScale(ParticleScale);
		}
	}
}

void UProjectileSpawnComponent::OnTimerEnd()
{
	if (!_projectile)
	{
		SpawnProjectile();
	}
}

void UProjectileSpawnComponent::SpawnProjectile(ElementType element, FVector endLocation)
{
	_endLocation = endLocation;
	if (_element != element)
	{
		_element = element;
		StopParticle();
	}

	if (!_projectile)
	{
		SpawnProjectile();
	}
}

void UProjectileSpawnComponent::Init(ElementType element)
{
	// spawn projectile for particle effect keeping in socket
	SpawnProjectile(element, FVector::Zero());
}

void UProjectileSpawnComponent::StopParticle()
{
	if (_projectile)
	{
		_projectile->Destroy();
		_projectile = nullptr;
	}
}

void UProjectileSpawnComponent::UpdateAnimInst()
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	UAnimInstance* animInst = character->GetMesh()->GetAnimInstance();

	animInst->OnPlayMontageNotifyBegin.AddDynamic(this, &UProjectileSpawnComponent::HandleOnMontageNotifyBegin);
}

ElementType UProjectileSpawnComponent::GetDefaultElementType()
{
	return DefaultElementType;
}
