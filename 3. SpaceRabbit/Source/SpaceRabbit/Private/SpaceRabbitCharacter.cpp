#include "SpaceRabbitCharacter.h"

#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "SpaceRabbitGameMode.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ASpaceRabbitCharacter::ASpaceRabbitCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->InitCapsuleSize(30.0f, 48.0f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbookComponent"));
	FlipbookComponent->SetupAttachment(CapsuleComponent);

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionFinder(TEXT("/Game/Input/IA_Jump.IA_Jump"));
	if (JumpActionFinder.Succeeded())
	{
		JumpAction = JumpActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextFinder(TEXT("/Game/Input/IMC_Player.IMC_Player"));
	if (MappingContextFinder.Succeeded())
	{
		InputMappingContext = MappingContextFinder.Object;
	}

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASpaceRabbitCharacter::BeginPlay()
{
	Super::BeginPlay();

	RegisterInputMappingContext();
	UpdateGroundedState();
	bPreviousGroundedState = bIsGrounded;
	PlayAnimationByGroundedState();
}

void ASpaceRabbitCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckDeathState();
	if (bIsDead)
	{
		return;
	}

	UpdateGroundedState();
	ApplyGravity(DeltaSeconds);
	ResolveVerticalMovement(DeltaSeconds);
	UpdateGroundedState();

	if (bPreviousGroundedState != bIsGrounded)
	{
		PlayAnimationByGroundedState();
	}

	bPreviousGroundedState = bIsGrounded;
	CheckDeathState();
}

void ASpaceRabbitCharacter::TriggerGameOver()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	bIsJumping = false;
	bIsGrounded = false;
	VerticalVelocity = 0.0f;

	if (ASpaceRabbitGameMode* SpaceRabbitGameMode = GetWorld() != nullptr ? GetWorld()->GetAuthGameMode<ASpaceRabbitGameMode>() : nullptr)
	{
		SpaceRabbitGameMode->SetGameOver();
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->SetPause(true);
		return;
	}

	UGameplayStatics::SetGamePaused(this, true);
}

void ASpaceRabbitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction != nullptr)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASpaceRabbitCharacter::OnJumpStarted);
		}
	}
}

void ASpaceRabbitCharacter::Jump()
{
	if (bIsDead || CurrentJumpCount >= MaxJumpCount)
	{
		return;
	}

	VerticalVelocity = JumpForce;
	++CurrentJumpCount;
	bIsGrounded = false;
	bIsJumping = true;
}

void ASpaceRabbitCharacter::ApplyGravity(float DeltaSeconds)
{
	if (bIsDead)
	{
		return;
	}

	if (bIsGrounded && VerticalVelocity <= 0.0f)
	{
		VerticalVelocity = 0.0f;
		return;
	}

	VerticalVelocity = FMath::Max(VerticalVelocity - (Gravity * DeltaSeconds), -MaxFallSpeed);
}

void ASpaceRabbitCharacter::CheckDeathState()
{
	if (GetActorLocation().Z <= DeathZ)
	{
		TriggerGameOver();
	}
}

void ASpaceRabbitCharacter::PlayAnimationByGroundedState()
{
	if (FlipbookComponent == nullptr)
	{
		return;
	}

	const ESpaceRabbitAnimationState DesiredAnimationState = bIsGrounded
		? ESpaceRabbitAnimationState::Run
		: ESpaceRabbitAnimationState::Jump;

	if (AnimationState == DesiredAnimationState)
	{
		return;
	}

	UPaperFlipbook* DesiredFlipbook = bIsGrounded ? RunFlipbook : JumpFlipbook;
	if (DesiredFlipbook == nullptr)
	{
		return;
	}

	AnimationState = DesiredAnimationState;
	FlipbookComponent->SetFlipbook(DesiredFlipbook);
	FlipbookComponent->PlayFromStart();
}

void ASpaceRabbitCharacter::RegisterInputMappingContext()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr)
	{
		PlayerController = GetWorld() != nullptr ? GetWorld()->GetFirstPlayerController() : nullptr;
	}

	if (PlayerController == nullptr || InputMappingContext == nullptr)
	{
		return;
	}

	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void ASpaceRabbitCharacter::UpdateGroundedState()
{
	if (bIsDead || CapsuleComponent == nullptr || GetWorld() == nullptr)
	{
		bIsGrounded = false;
		return;
	}

	const FVector ActorLocation = GetActorLocation();
	const float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	const FVector Start = ActorLocation;
	const FVector End = Start - FVector(0.0f, 0.0f, CapsuleHalfHeight + GroundCheckDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpaceRabbitGroundTrace), false, this);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

	const bool bWasGrounded = bIsGrounded;
	bIsGrounded = bHit && HitResult.ImpactNormal.Z > 0.5f && VerticalVelocity <= 0.0f;

	if (bIsGrounded)
	{
		CurrentJumpCount = 0;
		bIsJumping = false;

		if (!bWasGrounded)
		{
			const float DesiredActorZ = HitResult.ImpactPoint.Z + CapsuleHalfHeight;
			SetActorLocation(FVector(ActorLocation.X, ActorLocation.Y, DesiredActorZ));
		}
	}
}

void ASpaceRabbitCharacter::ResolveVerticalMovement(float DeltaSeconds)
{
	if (bIsDead || FMath::IsNearlyZero(VerticalVelocity))
	{
		return;
	}

	FHitResult HitResult;
	AddActorWorldOffset(FVector(0.0f, 0.0f, VerticalVelocity * DeltaSeconds), true, &HitResult);

	if (!HitResult.IsValidBlockingHit())
	{
		return;
	}

	if (HitResult.ImpactNormal.Z > 0.5f && VerticalVelocity <= 0.0f)
	{
		bIsGrounded = true;
		bIsJumping = false;
		CurrentJumpCount = 0;
	}

	VerticalVelocity = 0.0f;
}

void ASpaceRabbitCharacter::OnJumpStarted(const FInputActionValue& Value)
{
	if (!bIsDead && Value.Get<bool>())
	{
		Jump();
	}
}
