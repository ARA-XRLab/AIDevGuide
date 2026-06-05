#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "SpaceRabbitCharacter.generated.h"

class UCapsuleComponent;
class UInputAction;
class UInputMappingContext;
class UPaperFlipbook;
class UPaperFlipbookComponent;

UENUM()
enum class ESpaceRabbitAnimationState : uint8
{
	None,
	Run,
	Jump
};

UCLASS()
class SPACERABBIT_API ASpaceRabbitCharacter : public APawn
{
	GENERATED_BODY()

public:
	ASpaceRabbitCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void TriggerGameOver();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Jump();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ApplyGravity(float DeltaSeconds);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPaperFlipbookComponent> FlipbookComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UPaperFlipbook> RunFlipbook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UPaperFlipbook> JumpFlipbook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float JumpForce = 700.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float Gravity = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	int32 MaxJumpCount = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float GroundCheckDistance = 12.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxFallSpeed = 1800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game")
	float DeathZ = -500.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsJumping = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsGrounded = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

private:
	float VerticalVelocity = 0.0f;

	int32 CurrentJumpCount = 0;

	ESpaceRabbitAnimationState AnimationState = ESpaceRabbitAnimationState::None;

	bool bPreviousGroundedState = false;

	void CheckDeathState();
	void PlayAnimationByGroundedState();
	void RegisterInputMappingContext();
	void UpdateGroundedState();
	void ResolveVerticalMovement(float DeltaSeconds);
	void OnJumpStarted(const FInputActionValue& Value);
};
