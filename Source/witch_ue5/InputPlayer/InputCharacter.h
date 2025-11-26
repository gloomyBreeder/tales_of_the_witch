#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ActorComponents/CombatComponent.h"
#include "ActorComponents/AnimationComponent.h"
#include "ActorComponents/HealComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActorComponents/ProjectileSpawnComponent.h"
#include "Components/TimelineComponent.h"
#include "InputActionValue.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Interfaces/Interactable.h"
#include "Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "InputCharacter.generated.h"

UCLASS()
class WITCH_UE5_API AInputCharacter : public ACharacter, public IInteractable
{
	GENERATED_BODY()

public:
	AInputCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Init();
	void SavePosition();

	UFUNCTION(BlueprintCallable)
	ElementType GetCurrentElement();
	UFUNCTION(BlueprintCallable)
	bool IsMovingByInput();
	UFUNCTION(BlueprintCallable)
	bool IsInTelekinesis();
	UFUNCTION(BlueprintCallable)
	bool IsDoubleJumping();
	UFUNCTION(BlueprintCallable)
	bool CanDash();
	UFUNCTION(BlueprintCallable)
	int32 GetDashTimer();
	UFUNCTION(BlueprintCallable)
	bool CanTelekinesis();
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void HideUI();
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowUI();
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ShowInfoUI();

	void Rotate(FRotator rotation);
	void UnlockTelekinesis();
	void EnableAll(bool enable);
	void EnableOnlyMovement(bool enable);
	void Die();
	void Show(bool show);
	void SetWeaponState(bool show);
	void SetAnimationControl(bool on);

	virtual void InteractRequest_Implementation(AActor* interactableActor) override;
	virtual void OnEnterInteractionZone_Implementation(AActor* interactableActor, bool disableMovement, bool changeCamera) override;
	virtual void OnExitInteractionZone_Implementation(AActor* interactableActor) override;

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMode, uint8 PreviousCustomMode) override;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MeleeWeaponMesh;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* HealAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MeleeAttackAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* DistanceAttackAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* ChangeElementTypeAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* DashAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* SwitchTelekinesisAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, Category = "Jump")
	float DoubleJumpHeight = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Landed")
	float MinLandedHeight = 300.0f;
	UPROPERTY(EditAnywhere, Category = "Mouse Look")
	float DownLimitationPitch = -5.f;
	UPROPERTY(EditAnywhere, Category = "Mouse Look")
	float UpLimitationPitch = 15.f;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float ThrowForceMultiplier = 500.f;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float MaxControlDistance = 200.f;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	UNiagaraSystem* TelekinesisRayEffect;
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashCoeff = 2000.0f;
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashCooldownTime = 3.0f;
	UPROPERTY(EditAnywhere, Category = "Dash")
	float NextDashCooldownTime = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Dash")
	UNiagaraSystem* DashTrailEffect;
	UPROPERTY(EditAnywhere, Category = "Dash")
	UCurveFloat* DashCurve;
	UPROPERTY(EditAnywhere, Category = "Teleport back after falling")
	float TeleportHeight = -3000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
	float MaxTraceLength = 3000.f;
	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName SpellSocketName = FName("HandSocket");
	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName MeleeWeaponSocket = FName("MeleeWeaponSocket");
	UPROPERTY(EditAnywhere, Category = "Death")
	float DeathTimer = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	UNiagaraComponent* DashTrailComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	UNiagaraComponent* TelekinesisRayComponent;

	UFUNCTION()
	void DashUpdate(float Value);

private:
	void Move(const FInputActionValue& InputValue);
	void StopMoving();
	void Look(const FInputActionValue& InputValue);
	void Jump();
	void Heal();
	void MeleeAttack();
	void DistanceAttack();
	void ChangeElementType(const FInputActionValue& InputValue);
	void Dash();
	void Telekinesis();
	void CheckTelekinesisRay();
	void SwitchTelekinesisOn();
	void SwitchTelekinesisOff();
	void Interact();
	void SetupStimulusSource();
	void TeleportBack();
	void SaveLocation();
	void ResetDash();
	void Restore();
	FVector GetTraceLocation();
	void SetCameraFocus(bool enable);

	UCombatComponent* _combatComponent;
	UAnimationComponent* _animationComponent;
	UHealComponent* _healComponent;
	UProjectileSpawnComponent* _projectileSpawnComponent;
	UAIPerceptionStimuliSourceComponent* _stimulusSourceComponent;
	APlayerController* _playerController;
	UCharacterMovementComponent* _characterMovement;
	UCapsuleComponent* _capsuleComponent;

	AActor* _telekinesisTarget;
	IInteractable* _currentInteraction = nullptr;

	//for tracking enum ElementType
	int32 _currentElementType;
	int32 _elementsSize;
	int32 _jumpCount = 0;
	bool _isMovingByInput;
	bool _isFalling = false;
	bool _isInTelekinesis = false;
	bool _firstJump = true;
	bool _canTelekinesis = false;
	bool _disableActions = false;
	bool _disableMovement = false;
	bool _inited = false;
	bool _noWeapon = false;
	FVector _telekinesisDiff;
	FVector _telekinesisOffset;
	FVector _lastSafeLocation;
	FVector _afterKillLocation;
	FRotator _afterKillRotation;
	FVector2D _mouseDelta;
	float _mouseSpeed;
	float _elapsedTime;
	float _fallStartZ;

	//dash
	bool _canDash = true;
	bool _isDashing = false;
	FTimeline _dashTimeline;
	FVector _dashStartLocation;
	FVector _dashEndLocation;
	float _nextDashTime;
	int32 _lastDashWholeSecond = 0;
	FTimerHandle _dashCooldownTimerHandle;
	FTimerHandle _killTimerHandle;
};