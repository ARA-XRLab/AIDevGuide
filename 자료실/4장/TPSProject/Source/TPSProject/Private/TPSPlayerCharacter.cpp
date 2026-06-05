// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPlayerCharacter.h"

#include "TPSEnemyCharacter.h"
#include "TPSPlayerAnimInstance.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ATPSPlayerCharacter::ATPSPlayerCharacter()
{
	// 현재 단계에서는 매 프레임 처리할 내용이 없으므로 Tick을 비활성화합니다.
	PrimaryActorTick.bCanEverTick = false;

	// 캐릭터 몸통은 컨트롤러 회전을 직접 따라가지 않고 이동 방향을 바라보도록 설정합니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// 이동 방향을 기준으로 캐릭터가 자동 회전하도록 설정합니다.
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// 에디터에서 조정할 기본 카메라 거리를 설정합니다.
	CameraDistance = 400.0f;

	// 에디터에서 조정할 기본 스켈레탈 메시 위치 보정 값을 설정합니다.
	PlayerMeshRelativeLocation = FVector(0.0f, 0.0f, -90.0f);

	// 에디터에서 조정할 기본 스켈레탈 메시 회전 보정 값을 설정합니다.
	PlayerMeshRelativeRotation = FRotator(0.0f, -90.0f, 0.0f);

	// 에디터에서 조정할 기본 사격 판정 거리를 설정합니다.
	FireTraceDistance = 10000.0f;

	// 에디터에서 조정할 기본 사격 데미지를 설정합니다.
	FireDamage = 10.0f;

	// 총알 충돌 이펙트가 표면에 파묻히지 않도록 기본 띄움 거리를 설정합니다.
	BulletImpactFXOffset = 2.0f;

	// 마우스 시점 방향을 따라 회전하는 스프링 암을 생성합니다.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = CameraDistance;
	SpringArm->bUsePawnControlRotation = true;

	// 스프링 암 끝에 붙고 자체 회전은 따로 사용하지 않는 카메라를 생성합니다.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// 플레이어 몸통 캡슐은 이동 충돌만 담당하고 사격 판정에서는 제외합니다.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	// 코드에서 기본 스켈레탈 메시 슬롯에 Manny 메시를 직접 연결합니다.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyMeshAsset(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	if (MannyMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MannyMeshAsset.Object);
	}

	// 에디터에서 만든 플레이어 애니메이션 블루프린트 클래스를 기본 메시 슬롯에 연결합니다.
	static ConstructorHelpers::FClassFinder<UTPSPlayerAnimInstance> PlayerAnimBlueprintClass(TEXT("/Game/Animation/ABP_TPSPlayer"));
	if (PlayerAnimBlueprintClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(PlayerAnimBlueprintClass.Class);
	}

	// 플레이어 메시는 물리 충돌 없이 사격 판정만 받도록 설정합니다.
	GetMesh()->SetRelativeLocation(PlayerMeshRelativeLocation);
	GetMesh()->SetRelativeRotation(PlayerMeshRelativeRotation);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(false);
	GetMesh()->SetSimulatePhysics(false);

	// 플레이어 오른손 소켓에 따라 움직일 총기 스켈레탈 메시 컴포넌트를 생성합니다.
	RifleMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleMeshComp"));
	RifleMeshComp->SetupAttachment(GetMesh(), TEXT("HandGrip_R"));

	// 코드에서 총기 스켈레탈 메시를 직접 연결합니다.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> RifleMeshAsset(TEXT("/Game/Weapons/Rifle/Meshes/SKM_Rifle.SKM_Rifle"));
	if (RifleMeshAsset.Succeeded())
	{
		RifleMeshComp->SetSkeletalMesh(RifleMeshAsset.Object);
	}

	// 총기는 연출용이므로 모든 충돌을 비활성화합니다.
	RifleMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RifleMeshComp->SetGenerateOverlapEvents(false);
	RifleMeshComp->SetSimulatePhysics(false);
}

void ATPSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 플레이어의 Enhanced Input Subsystem에 기본 매핑 컨텍스트를 등록합니다.
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}

		// 로컬 플레이어 화면에 크로스헤어 위젯을 생성해 표시합니다.
		if (PlayerController->IsLocalController() && CrosshairWidgetClass)
		{
			CrosshairWidget = CreateWidget<UUserWidget>(PlayerController, CrosshairWidgetClass);
			if (CrosshairWidget)
			{
				CrosshairWidget->AddToViewport();
			}
		}
	}
}

void ATPSPlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 에디터에서 조정한 카메라 거리를 스프링 암 길이에 반영합니다.
	SpringArm->TargetArmLength = CameraDistance;

	// 에디터에서 조정한 스켈레탈 메시 보정 값을 기본 메시 슬롯에 반영합니다.
	GetMesh()->SetRelativeLocation(PlayerMeshRelativeLocation);
	GetMesh()->SetRelativeRotation(PlayerMeshRelativeRotation);
}

void ATPSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input 컴포넌트에 이동과 시점 회전 액션을 연결합니다.
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPSPlayerCharacter::Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPSPlayerCharacter::Look);
		}

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		}

		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATPSPlayerCharacter::Fire);
		}
	}
}

void ATPSPlayerCharacter::Move(const FInputActionValue& Value)
{
	// Move 입력 값의 X는 앞뒤, Y는 좌우 이동에 사용합니다.
	const FVector2D MoveValue = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MoveValue.X);
		AddMovementInput(RightDirection, MoveValue.Y);
	}
}

void ATPSPlayerCharacter::Look(const FInputActionValue& Value)
{
	// Look 입력 값의 X는 좌우 회전, Y는 상하 회전에 사용합니다.
	const FVector2D LookValue = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookValue.X);
		AddControllerPitchInput(-LookValue.Y);
	}
}

void ATPSPlayerCharacter::Fire()
{
	if (UTPSPlayerAnimInstance* PlayerAnimInstance = Cast<UTPSPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		PlayerAnimInstance->PlayAttackAnim();
	}

	if (BulletSound)
	{
		UGameplayStatics::PlaySound2D(this, BulletSound);
	}

	// 카메라 위치와 바라보는 방향을 기준으로 발사 시작점과 끝점을 계산합니다.
	const FVector TraceStart = Camera->GetComponentLocation();
	const FVector FireDirection = Camera->GetForwardVector();
	const FVector TraceEnd = TraceStart + (FireDirection * FireTraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(Fire), false, this);
	QueryParams.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	const FVector DebugEnd = bHit ? HitResult.ImpactPoint : TraceEnd;

	DrawDebugLine(GetWorld(), TraceStart, DebugEnd, FColor::Yellow, false, 2.0f, 0, 2.0f);

	if (bHit)
	{
		DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 12.0f, FColor::Red, false, 2.0f);
		SpawnBulletImpactFX(HitResult);

		if (AActor* HitActor = HitResult.GetActor())
		{
			ApplyDamageToHitEnemy(HitActor);
			UE_LOG(LogTemp, Warning, TEXT("Fire Hit Actor: %s"), *HitActor->GetName());
		}
	}
}

void ATPSPlayerCharacter::SpawnBulletImpactFX(const FHitResult& HitResult)
{
	// 이펙트가 연결되지 않은 경우에는 조용히 건너뜁니다.
	if (!BulletImpactFX)
	{
		return;
	}

	const FVector SpawnLocation = HitResult.ImpactPoint + (HitResult.ImpactNormal * BulletImpactFXOffset);
	const FRotator SpawnRotation = HitResult.ImpactNormal.Rotation();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactFX, SpawnLocation, SpawnRotation);
}

void ATPSPlayerCharacter::ApplyDamageToHitEnemy(AActor* HitActor)
{
	// 맞은 대상이 적 캐릭터가 아니면 데미지를 전달하지 않습니다.
	if (ATPSEnemyCharacter* HitEnemy = Cast<ATPSEnemyCharacter>(HitActor))
	{
		UGameplayStatics::ApplyDamage(HitEnemy, FireDamage, GetController(), this, UDamageType::StaticClass());
	}
}
