#include "InputCharacter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Static/UtilityHelper.h"
#include "Static/GameStateManager.h"
#include "ActorComponents/TelekinesisMovableComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/AISense_Sight.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void AInputCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	_isFalling = false;
	_firstJump = true;
	_jumpCount = 0;
	SaveLocation();

	if (_inited)
	{
		float fallHeight = _fallStartZ - GetActorLocation().Z;

		if (fallHeight >= MinLandedHeight)
		{
			Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EPlayerSoundType::Land, GetActorLocation());
		}
		_fallStartZ = 0.0f;
	}
}

void AInputCharacter::OnMovementModeChanged(EMovementMode PrevMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMode, PreviousCustomMode);

    if (_characterMovement && _characterMovement->MovementMode == MOVE_Falling)
    {
        _fallStartZ = GetActorLocation().Z;
    }
}

AInputCharacter::AInputCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	MeleeWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Melee Weapon Mesh"));
	MeleeWeaponMesh->SetupAttachment(GetMesh(), MeleeWeaponSocket);

	// true - spin with cam, false - no 
	//bUseControllerRotationYaw = false;
	//_characterMovement->bOrientRotationToMovement = false;

	SetupStimulusSource();
}

void AInputCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AInputCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (_isDashing)
	{
		_dashTimeline.TickTimeline(DeltaTime);
	}

	// teleport if falling too long
	if (_characterMovement->IsFalling() && GetVelocity().Z <= TeleportHeight && !_isFalling)
	{
		_isFalling = true;
		TeleportBack();
	}
	else
	{
		// save coordinates of player before falling with delay
		if (_elapsedTime >= 3.0f)
		{
			SaveLocation();
		}
		else
		{
			_elapsedTime += DeltaTime;
		}
	}

	if (!_canDash)
	{
		_nextDashTime += DeltaTime;

		if (_nextDashTime >= NextDashCooldownTime)
		{
			Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->StopSound(UtilityHelper::EnumToFName(EPlayerSoundType::DashTimer));
			_canDash = true;
		}
	}

	if (_isInTelekinesis)
	{
		Telekinesis();
	}
}

void AInputCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	_characterMovement = GetCharacterMovement();
	_combatComponent = FindComponentByClass<UCombatComponent>();
	_animationComponent = FindComponentByClass<UAnimationComponent>();
	_healComponent = FindComponentByClass<UHealComponent>();
	_projectileSpawnComponent = FindComponentByClass<UProjectileSpawnComponent>();
	_capsuleComponent = GetCapsuleComponent();

	//set default element
	_currentElementType = static_cast<int>(ElementOrder[0]);
	_elementsSize = ElementOrder.Num();
	ElementType currentElement = GetCurrentElement();
	_combatComponent->SetAttackStatsElement(currentElement);
	_projectileSpawnComponent->Init(currentElement);

	if (TelekinesisRayEffect)
	{
		TelekinesisRayComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TelekinesisRayEffect,
			GetMesh(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			false);
	}

	if (DashTrailEffect)
	{
		DashTrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			DashTrailEffect,
			GetMesh(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			false);
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if (TelekinesisRayComponent)
			{
				TelekinesisRayComponent->Deactivate();
			}

			if (DashTrailComponent)
			{
				DashTrailComponent->Deactivate();
			}
		}, 0.5f, false);

	if (DashCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("DashUpdate"));
		_dashTimeline.AddInterpFloat(DashCurve, ProgressFunction);
		_dashTimeline.SetTimelineLength(DashCooldownTime);
	}
}

void AInputCharacter::SaveLocation()
{
	if (GetVelocity().Z > TeleportHeight && !_characterMovement->IsFalling() && !_isFalling)
	{
		_lastSafeLocation = GetActorLocation();
		_elapsedTime = 0.f;
		/*GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
			FString::Printf(TEXT("save location: %s"),
				*_lastSafeLocation.ToString()));*/
	}
}

void AInputCharacter::ResetDash()
{
	_isDashing = false;
	//set visible again
	GetMesh()->SetVisibility(true);
	if (DashTrailComponent)
	{
		DashTrailComponent->Deactivate();
	}

	if (MeleeWeaponMesh)
	{
		MeleeWeaponMesh->SetVisibility(true);
	}

	if (_projectileSpawnComponent)
	{
		_projectileSpawnComponent->SpawnProjectile(GetCurrentElement(), FVector::Zero());
	}

	_dashTimeline.Stop();
}

void AInputCharacter::Restore()
{
	SetActorLocation(_afterKillLocation);
	Rotate(_afterKillRotation);
	_characterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
	_combatComponent->Restore();
	_healComponent->Restore();
	if (_projectileSpawnComponent)
	{
		_projectileSpawnComponent->SpawnProjectile(GetCurrentElement(), FVector::Zero());
	}
	Show(true);
}

FVector AInputCharacter::GetTraceLocation()
{
	FVector cameraLocation = Camera->GetComponentLocation();
	FVector forwardVector = Camera->GetForwardVector();
	FVector traceEnd = cameraLocation + forwardVector * MaxTraceLength;

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);

	bool wasHit = GetWorld()->LineTraceSingleByChannel(hitResult, cameraLocation, traceEnd, ECC_Visibility, queryParams);

	if (wasHit)
	{
		return hitResult.Location;
	}
	else
	{
		return traceEnd;
	}
}

void AInputCharacter::SetCameraFocus(bool enable)
{
	if (enable && _currentInteraction)
	{
		// Camera locks to NPC
		_playerController->SetViewTargetWithBlend(Cast<AActor>(_currentInteraction), 0.5f);
	}
	else
	{
		// fix camera trick
		if (_playerController->PlayerCameraManager)
		{
			_playerController->PlayerCameraManager->SetManualCameraFade(1.0f, FLinearColor::Black, false);
			_playerController->SetViewTarget(nullptr);
			_playerController->SetViewTarget(this);
			_playerController->PlayerCameraManager->SetManualCameraFade(0.0f, FLinearColor::Black, false);
		}
	}
}

void AInputCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	_playerController = Cast<APlayerController>(Controller);
	if (_playerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_playerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInputCharacter::Move);
		Input->BindAction(MoveAction, ETriggerEvent::Completed, this, &AInputCharacter::StopMoving);
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &AInputCharacter::Jump);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInputCharacter::Look);
		Input->BindAction(HealAction, ETriggerEvent::Started, this, &AInputCharacter::Heal);
		Input->BindAction(MeleeAttackAction, ETriggerEvent::Started, this, &AInputCharacter::MeleeAttack);
		Input->BindAction(DistanceAttackAction, ETriggerEvent::Started, this, &AInputCharacter::DistanceAttack);
		Input->BindAction(ChangeElementTypeAction, ETriggerEvent::Triggered, this, &AInputCharacter::ChangeElementType);
		Input->BindAction(DashAction, ETriggerEvent::Triggered, this, &AInputCharacter::Dash);
		Input->BindAction(InteractAction, ETriggerEvent::Started, this, &AInputCharacter::Interact);
		Input->BindAction(SwitchTelekinesisAction, ETriggerEvent::Triggered, this, &AInputCharacter::SwitchTelekinesisOn);
		Input->BindAction(SwitchTelekinesisAction, ETriggerEvent::Completed, this, &AInputCharacter::SwitchTelekinesisOff);
	}
}

void AInputCharacter::Init()
{
	APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (cameraManager)
	{
		cameraManager->ViewPitchMin = DownLimitationPitch;
		cameraManager->ViewPitchMax = UpLimitationPitch;
	}
}

void AInputCharacter::SavePosition()
{
	_inited = true;
	_afterKillLocation = GetActorLocation();
	_afterKillRotation = GetActorRotation();
}

void AInputCharacter::Move(const FInputActionValue& InputValue)
{
	if (_disableMovement)
	{
		return;
	}

	_characterMovement->bOrientRotationToMovement = false;
	FVector2D InputVector = InputValue.Get<FVector2D>();
	_isMovingByInput = !InputVector.IsZero();

	if (IsValid(Controller))
	{
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator yawRotation(0, rotation.Yaw, 0);

		// get forward direction
		const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		//right direction
		const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		//add movement input
		AddMovementInput(forwardDirection, InputVector.Y);
		AddMovementInput(rightDirection, InputVector.X);

		// Compute the movement direction
		FVector movementDirection = (forwardDirection * InputVector.Y) + (rightDirection * InputVector.X);
		movementDirection = movementDirection.GetSafeNormal();  // Normalize to prevent scaling issues
		float DotProduct = FVector::DotProduct(forwardDirection, movementDirection);
		bool isMovingBackward = (DotProduct < -0.5f);

		//player rotation when jump except jump back
		if (_isMovingByInput && _characterMovement->IsFalling() && !isMovingBackward && !movementDirection.IsZero())
		{
			FRotator movementRotation = movementDirection.ToOrientationRotator();
			SetActorRotation(FRotator(0.0f, movementRotation.Yaw, 0.0f));  // Rotate to face movement
		}
	}
}

void AInputCharacter::StopMoving()
{
	_isMovingByInput = false;
	_characterMovement->bOrientRotationToMovement = true;
}

void AInputCharacter::Look(const FInputActionValue& InputValue)
{
	if (_disableMovement)
	{
		return;
	}

	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);

		_mouseDelta = InputVector;
		_mouseSpeed = _mouseDelta.Size();
	}
}

void AInputCharacter::Jump()
{
	if (_disableMovement)
	{
		return;
	}

	if (_isDashing)
	{
		return;
	}

	//fix rotation
	//if (!_isMovingByInput)
	//{
	//	// If idle, rotate to face the camera direction
	//	FRotator CameraRotation = Controller->GetControlRotation();
	//	SetActorRotation(FRotator(0.0f, CameraRotation.Yaw, 0.0f));

	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
	//		FString::Printf(TEXT("CameraRotation: %s"),
	//			*CameraRotation.ToString()));
	//}
	//

	if (_firstJump)
	{
		_firstJump = false;
		Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EPlayerSoundType::Jump, GetActorLocation());
		ACharacter::Jump();
	}
	else
	{
		if (_jumpCount == 1)
		{
			Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EPlayerSoundType::DoubleJump, GetActorLocation());
			LaunchCharacter(FVector(0.0f, 0.f, DoubleJumpHeight), false, true);
		}
	}

	_jumpCount++;
	_characterMovement->bOrientRotationToMovement = true;
}

void AInputCharacter::Heal()
{
	if (_disableActions)
	{
		return;
	}

	if (_combatComponent->IsDoingAction())
	{
		return;
	}

	if (_healComponent)
	{
		if (_healComponent->CanHeal())
		{
			_healComponent->InitialHeal();

			if (_projectileSpawnComponent)
			{
				_projectileSpawnComponent->StopParticle();
			}
		}
	}
}

void AInputCharacter::MeleeAttack()
{
	if (_disableActions)
	{
		return;
	}

	if (_isDashing)
	{
		return;
	}

	_combatComponent->Attack();
}

void AInputCharacter::ChangeElementType(const FInputActionValue& InputValue)
{
	if (_disableActions)
	{
		return;
	}

	if (_isDashing)
	{
		return;
	}

	float InputFloat = InputValue.Get<float>();

	if (InputFloat < 0)
	{
		//mouse wheel down
		if (_currentElementType == _elementsSize)
		{
			_currentElementType = 1;
		}
		else
		{
			_currentElementType++;
		}
	}
	else
	{
		//mouse wheel up
		if (_currentElementType == 1)
		{
			_currentElementType = _elementsSize;
		}
		else
		{
			_currentElementType--;
		}
	}

	ElementType currentElement = GetCurrentElement();
	_projectileSpawnComponent->Init(currentElement);
	_combatComponent->SetAttackStatsElement(currentElement);
	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(currentElement);
}

void AInputCharacter::Dash()
{
	if (_disableActions)
	{
		return;
	}

	if (!_canDash)
	{
		return;
	}

	if (_characterMovement->IsFalling())
	{
		return;
	}

	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EPlayerSoundType::Dash, GetActorLocation());
	_canDash = false;
	_nextDashTime = 0.f;
	_lastDashWholeSecond = 0;
	_isDashing = true;
	GetMesh()->SetVisibility(false);

	if (DashTrailComponent)
	{
		DashTrailComponent->Activate();
	}
	if (_projectileSpawnComponent)
	{
		_projectileSpawnComponent->StopParticle();
	}
	if (MeleeWeaponMesh)
	{
		MeleeWeaponMesh->SetVisibility(false);
	}

	_characterMovement->StopActiveMovement();

	FVector dashDirection = GetLastMovementInputVector();
	if (dashDirection.IsNearlyZero())  // Default to forward direction if no input is detected
	{
		dashDirection = GetActorForwardVector();  // Use forward direction as a fallback
	}

	dashDirection.Normalize();
	dashDirection.Z = 0;
	_dashStartLocation = GetActorLocation();
	_dashEndLocation = _dashStartLocation + (dashDirection * DashCoeff);

	if (!_dashTimeline.IsPlaying())
	{
		_dashTimeline.PlayFromStart();
	}

	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EPlayerSoundType::DashTimer, GetActorLocation(), true);
	GetWorldTimerManager().SetTimer(_dashCooldownTimerHandle, this, &AInputCharacter::ResetDash, DashCooldownTime, false);
}

void AInputCharacter::DashUpdate(float Value)
{
	FVector NewLocation = FMath::Lerp(_dashStartLocation, _dashEndLocation, Value);
	SetActorLocation(NewLocation, true);
}

void AInputCharacter::DistanceAttack()
{
	if (_disableActions)
	{
		return;
	}

	if (_isDashing)
	{
		return;
	}

	// Get the crosshair target location
	FVector targetLocation = GetTraceLocation();  

	// Get the hand socket location (where the fireball is spawned)
	USkeletalMeshComponent* meshComp = GetMesh();
	FVector handSocketLocation = meshComp->GetSocketLocation(SpellSocketName);

	// Calculate the direction from the hand to the target
	FVector launchDirection = (targetLocation - handSocketLocation).GetSafeNormal();
	FVector direction = handSocketLocation + launchDirection * MaxTraceLength;

	//rotate idle player in the direction of casting (can cast only in the center of crosshair)
	if (!_isMovingByInput)
	{
		FRotator newRotation = (direction - GetActorLocation()).Rotation();
		SetActorRotation(newRotation);
	}

	//DrawDebugLine(GetWorld(), handSocketLocation, direction, FColor::Red, false, 2.0f, 0, 1.0f);

	_combatComponent->CastSpell(GetCurrentElement(), direction);
}

void AInputCharacter::CheckTelekinesisRay()
{
	USkeletalMeshComponent* meshComp = GetMesh();
	FVector handSocketLocation = meshComp->GetSocketLocation(SpellSocketName);

	FVector cameraLocation = Camera->GetComponentLocation();
	FVector forwardVector = Camera->GetForwardVector();
	FVector traceEnd = cameraLocation + forwardVector * MaxTraceLength;

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);

	bool wasHit = GetWorld()->LineTraceSingleByChannel(hitResult, handSocketLocation, traceEnd, ECC_Visibility, queryParams);

	//DrawDebugLine(GetWorld(), HandSocketLocation, TraceEnd, FColor::Red, false, 2.0f, 0, 1.0f);

	FRotator newRotation = (traceEnd - GetActorLocation()).Rotation();
	SetActorRotation(newRotation);

	if (TelekinesisRayComponent)
	{
		TelekinesisRayComponent->SetVectorParameter(TEXT("BeamStart"), handSocketLocation);
		TelekinesisRayComponent->SetVectorParameter(TEXT("BeamEnd"), traceEnd);
		TelekinesisRayComponent->SetEmitterEnable(TEXT("RayParticles"), false);
	}

	if (wasHit)
	{
		//DrawDebugLine(GetWorld(), HandSocketLocation, HitResult.Location, FColor::Blue, false, 2.0f, 0, 1.0f);

		UStaticMeshComponent* meshComponent = hitResult.GetActor()->FindComponentByClass<UStaticMeshComponent>();
		UTelekinesisMovableComponent* telekinesisComponent = hitResult.GetActor()->FindComponentByClass<UTelekinesisMovableComponent>();
		if (meshComponent && telekinesisComponent)
		{
			_telekinesisTarget = hitResult.GetActor();
			_telekinesisDiff = Camera->GetComponentLocation() - hitResult.Location;
			_telekinesisOffset = hitResult.Location - _telekinesisTarget->GetActorLocation();
			meshComponent->SetSimulatePhysics(false);
		}
	}
}

void AInputCharacter::Telekinesis()
{
	if (!_telekinesisTarget)
	{
		CheckTelekinesisRay();
	}

	if (_telekinesisTarget)
	{
		FVector handSocketLocation = GetMesh()->GetSocketLocation(SpellSocketName);
		FVector cameraLocation = Camera->GetComponentLocation();
		FVector forwardVector = Camera->GetForwardVector();
		FVector newLocation = cameraLocation + forwardVector * _telekinesisDiff.Size();
		FVector newLocationWithOffset = newLocation - _telekinesisOffset;

		if (TelekinesisRayComponent)
		{
			TelekinesisRayComponent->SetVectorParameter(TEXT("BeamStart"), handSocketLocation);
			TelekinesisRayComponent->SetVectorParameter(TEXT("BeamEnd"), newLocationWithOffset);
			TelekinesisRayComponent->SetEmitterEnable(TEXT("RayParticles"), true);
		}

		// Смещение объекта для выравнивания с точкой удара
		//_telekinesisTarget->SetActorLocation(NewLocationWithOffset);

		float distanceToRay = FVector::Distance(newLocationWithOffset, _telekinesisTarget->GetActorLocation());

		// If the target is too far from the ray, lose control
		if (distanceToRay > MaxControlDistance)
		{
			UStaticMeshComponent* meshComp = _telekinesisTarget->FindComponentByClass<UStaticMeshComponent>();
			if (meshComp)
			{
				meshComp->SetSimulatePhysics(true);
			}
			_telekinesisTarget = nullptr;
			return;
		}

		// Move the telekinesis target with collision handling
		FHitResult hitResult;
		bool wasHit = _telekinesisTarget->SetActorLocation(newLocationWithOffset, true, &hitResult);

		if (!wasHit)
		{
			// Object collided with something: freeze position
			FVector ImpactNormal = hitResult.Normal;  // Direction of the surface we hit
			FVector FrozenLocation = hitResult.Location;  // Where the object was stopped

			// Adjust position to prevent dragging through obstacles
			FVector PushBackDirection = (FrozenLocation - newLocationWithOffset).GetSafeNormal();
			FVector AdjustedPosition = FrozenLocation + PushBackDirection * 10.0f;

			_telekinesisTarget->SetActorLocation(AdjustedPosition, false);  // Move target slightly back
		}

		//DrawDebugLine(GetWorld(), CameraLocation, NewLocation, FColor::Green, false, 2.0f, 0, 1.0f);

		FRotator newRotation = (newLocation - GetActorLocation()).Rotation();
		SetActorRotation(newRotation);
	}

}

void AInputCharacter::SwitchTelekinesisOn()
{
	if (!_canTelekinesis)
	{
		return;
	}

	if (_disableActions)
	{
		return;
	}

	_isInTelekinesis = true;
	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->PlaySound(EPlayerSoundType::Telekinesis, GetActorLocation(), true);

	if (_projectileSpawnComponent)
	{
		_projectileSpawnComponent->StopParticle();
	}

	_characterMovement->DisableMovement();

	if (TelekinesisRayComponent)
	{
		TelekinesisRayComponent->Activate();
	}
}

void AInputCharacter::SwitchTelekinesisOff()
{
	if (!_canTelekinesis)
	{
		return;
	}

	if (_disableActions)
	{
		return;
	}

	_isInTelekinesis = false;
	Cast<AGameStateManager>(GetWorld()->GetGameState())->GetSoundManager()->StopSound(UtilityHelper::EnumToFName(EPlayerSoundType::Telekinesis));

	if (_projectileSpawnComponent)
	{
		_projectileSpawnComponent->SpawnProjectile(GetCurrentElement(), FVector::Zero());
	}

	if (TelekinesisRayComponent)
	{
		TelekinesisRayComponent->Deactivate();
	}

	_characterMovement->SetMovementMode(EMovementMode::MOVE_Walking);

	//throw target away!
	if (_telekinesisTarget)
	{
		UStaticMeshComponent* meshComp = _telekinesisTarget->FindComponentByClass<UStaticMeshComponent>();
		if (meshComp)
		{
			meshComp->SetSimulatePhysics(true);
			FVector launchDirection = Camera->GetForwardVector() + FVector(_mouseDelta.X, _mouseDelta.Y, 0.0f).GetSafeNormal();
			float strength = _mouseSpeed * ThrowForceMultiplier;
			meshComp->AddImpulse(launchDirection * strength, NAME_None, true);
		}

		_telekinesisTarget = nullptr;
	}
}

void AInputCharacter::Interact()
{
	if (_currentInteraction)
	{
		IInteractable::Execute_InteractRequest(Cast<AActor>(_currentInteraction), this);
	}
}

void AInputCharacter::SetupStimulusSource()
{
	_stimulusSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (_stimulusSourceComponent)
	{
		_stimulusSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
		_stimulusSourceComponent->RegisterWithPerceptionSystem();
	}
}

void AInputCharacter::TeleportBack()
{
	if (_isFalling)
	{
		// todo test this fix for stucking in textures
		// maybe mafth clamp to test minimun
		//_lastSafeLocation.Z += 100;
		SetActorLocation(_lastSafeLocation);
		_isFalling = false;
	}

}

ElementType AInputCharacter::GetCurrentElement()
{
	return static_cast<ElementType>(_currentElementType);
}

bool AInputCharacter::IsMovingByInput()
{
	return _isMovingByInput;
}

bool AInputCharacter::IsInTelekinesis()
{
	return _isInTelekinesis;
}

bool AInputCharacter::IsDoubleJumping()
{
	return _jumpCount == 2;
}

bool AInputCharacter::CanDash()
{
	return _canDash;
}

int32 AInputCharacter::GetDashTimer()
{
	int32 currentWholeSecond = FMath::FloorToInt(_nextDashTime);

	if (currentWholeSecond > _lastDashWholeSecond)
	{
		_lastDashWholeSecond = currentWholeSecond;
		return currentWholeSecond;
	}

	return _lastDashWholeSecond;
}

bool AInputCharacter::CanTelekinesis()
{
	return _canTelekinesis;
}

void AInputCharacter::Rotate(FRotator rotation)
{
	SetActorRotation(rotation);
	GetController()->SetControlRotation(GetActorRotation());
}

void AInputCharacter::UnlockTelekinesis()
{
	_canTelekinesis = true;
}

void AInputCharacter::InteractRequest_Implementation(AActor* InteractableActor)
{
	if (InteractableActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
	}
}

void AInputCharacter::OnEnterInteractionZone_Implementation(AActor* interactableActor, bool disableMovement, bool changeCamera)
{
	if (interactableActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		EnableAll(!disableMovement);
		_currentInteraction = Cast<IInteractable>(interactableActor);

		if (_disableMovement || _disableActions)
		{
			_characterMovement->Velocity = FVector::Zero();
			if (changeCamera)
			{
				SetCameraFocus(true);
			}
		}
	}
}

void AInputCharacter::OnExitInteractionZone_Implementation(AActor* interactableActor)
{
	if (interactableActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		_currentInteraction = nullptr;
		SetCameraFocus(false);
		EnableAll(true);
	}
}

void AInputCharacter::EnableAll(bool enable)
{
	if (_noWeapon)
	{
		_disableActions = true;
	}
	else
	{
		_disableActions = !enable;
	}

	_disableMovement = !enable;
}

void AInputCharacter::EnableOnlyMovement(bool enable)
{
	_disableMovement = !enable;
	if (enable)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	else
	{
		GetCharacterMovement()->DisableMovement();
	}
}

void AInputCharacter::Die()
{
	Show(false);
	GetWorldTimerManager().SetTimer(_killTimerHandle, this, &AInputCharacter::Restore, 0.5f, false);
}

void AInputCharacter::Show(bool show)
{
	GetMesh()->SetVisibility(show);

	SetWeaponState(show);

	if (show)
	{
		_characterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	else
	{
		_characterMovement->DisableMovement();
	}
}

void AInputCharacter::SetWeaponState(bool show)
{
	_noWeapon = !show;

	if (MeleeWeaponMesh)
	{
		MeleeWeaponMesh->SetVisibility(show);
	}

	if (_projectileSpawnComponent)
	{
		if (show)
		{
			_projectileSpawnComponent->UpdateAnimInst();
			_projectileSpawnComponent->SpawnProjectile(GetCurrentElement(), FVector::Zero());

		}
		else
		{
			_projectileSpawnComponent->StopParticle();
		}
	}
}

void AInputCharacter::SetAnimationControl(bool on)
{
	USkeletalMeshComponent* meshComponent = GetMesh();
	if (!meshComponent)
	{
		return;
	}

	if (on)
	{
		meshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		_animationComponent->UpdateAnimInst();
	}
	else
	{
		meshComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	}
}
