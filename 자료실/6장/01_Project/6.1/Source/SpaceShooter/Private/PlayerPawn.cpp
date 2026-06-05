// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"

#include "PlayerBulletActor.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "ShootingGameMode.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Shooting/Models/SpaceShooter/SpaceShooter.SpaceShooter"));
	if (MeshFinder.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshFinder.Object);
	}
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/Shooting/Models/SpaceShooter/MI_SpaceShooter.MI_SpaceShooter"));
	if (MaterialFinder.Succeeded())
	{
		MeshComp->SetMaterial(0, MaterialFinder.Object);
	}

	FirePointComp = CreateDefaultSubobject<UArrowComponent>(TEXT("FirePointComp"));
	FirePointComp->SetupAttachment(RootComponent);

	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionProfileName(TEXT("Player"));

	ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionVFXFinder(TEXT("/Game/Shooting/Particles/P_Explosion.P_Explosion"));
	if (ExplosionVFXFinder.Succeeded())
	{
		ExplosionVFX = ExplosionVFXFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundCue> FireSoundFinder(TEXT("/Game/Shooting/Sound/PlayerBullet.PlayerBullet"));
	if (FireSoundFinder.Succeeded())
	{
		FireSound = FireSoundFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundCue> ExplosionSoundFinder(TEXT("/Game/Shooting/Sound/Explosion.Explosion"));
	if (ExplosionSoundFinder.Succeeded())
	{
		ExplosionSound = ExplosionSoundFinder.Object;
	}
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (InputSubsystem)
	{
		InputSubsystem->AddMappingContext(IMC_Player, 0);
	}

	if (AShootingGameMode* GameMode = Cast<AShootingGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->UpdatePlayerHP(CurHP);
	}

	CurHP = MaxHP;

	UMaterialInterface* BaseMaterial = MeshComp->GetMaterial(0);
	if (BaseMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		MeshComp->SetMaterial(0, DynamicMaterial);
	}

	BulletPool.Empty(MaxBulletCount);
	for (int32 i = 0; i < MaxBulletCount; i++)
	{
		ABulletActor* Bullet = GetWorld()->SpawnActor<ABulletActor>(BulletFactory);
		Bullet->SetActive(false);
		BulletPool.Add(Bullet);
	}
}

void APlayerPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	GetWorldTimerManager().ClearTimer(InvincibilityTimerHandle);
	GetWorldTimerManager().ClearTimer(BlinkTimerHandle);
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	FVector MoveDirection = FVector(0, InputH, InputV);
	MoveDirection.Normalize();
	FVector Velocity = MoveDirection * Speed;

	FHitResult Hit;
	SetActorLocation(CurrentLocation + Velocity * DeltaTime, true, &Hit);
	
	float TargetTilt = InputH * TiltAngle;
	FRotator CurrentRotation = MeshComp->GetRelativeRotation();
	FRotator TargetRotation = FRotator(CurrentRotation.Pitch, 180.f + TargetTilt, CurrentRotation.Roll);
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, TiltInterpSpeed);
	MeshComp->SetRelativeRotation(NewRotation);

	InputH = 0.f;
	InputV = 0.f;
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInput)
	{
		EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerPawn::OnInputMove);
		
		EnhancedInput->BindAction(IA_Fire, ETriggerEvent::Started, this, &APlayerPawn::OnInputFirePressed);
		EnhancedInput->BindAction(IA_Fire, ETriggerEvent::Completed, this, &APlayerPawn::OnInputFireReleased);
	}
}

void APlayerPawn::OnInputMove(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();
	InputV = InputValue.X;
	InputH = InputValue.Y;
}

void APlayerPawn::FireBullet()
{
	ABulletActor* Bullet = nullptr;

	if (BulletPool.Num() > 0)
	{
		Bullet = BulletPool[0];
		BulletPool.RemoveAt(0);
	}
	else
	{
		Bullet = GetWorld()->SpawnActor<ABulletActor>(BulletFactory);
	}

	if (Bullet)
	{
		FTransform SpawnTransform = FirePointComp->GetComponentTransform();
		Bullet->SetActorTransform(SpawnTransform);
		Bullet->SetActive(true);

		UGameplayStatics::PlaySound2D(GetWorld(), FireSound);
	}
}

int32 APlayerPawn::GetHP()
{
	return CurHP;
}

void APlayerPawn::SetHP(int32 NewHP)
{
	CurHP = NewHP;
	if (AShootingGameMode* GameMode = Cast<AShootingGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->UpdatePlayerHP(CurHP);
	}

	if (CurHP <= 0)
	{
		Die();
	}
}

void APlayerPawn::Die()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	if (AShootingGameMode* GameMode = Cast<AShootingGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->ShowGameOverUI();
	}

	Destroy();
}

void APlayerPawn::OnDamage(int32 Amount)
{
	if (bIsInvincible)
	{
		return;
	}

	SetHP(CurHP - Amount);
	StartInvincibility();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation());
	UGameplayStatics::PlaySound2D(GetWorld(), ExplosionSound);
}

bool APlayerPawn::IsInvincible() const
{
	return bIsInvincible;
}

void APlayerPawn::StartInvincibility()
{
	bIsInvincible = true;

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("HitIntensity"), 1.f);
	}

	// Lambda 캡처 대신 UObject 바인딩을 사용해 수명/호출 안정성을 확보합니다.
	GetWorldTimerManager().SetTimer(
		BlinkTimerHandle,
		this,
		&APlayerPawn::BlinkMesh,
		BlinkInterval,
		true
	);

	GetWorldTimerManager().SetTimer(
		InvincibilityTimerHandle,
		this,
		&APlayerPawn::EndInvincibility,
		InvincibilityDuration,
		false
	);

}

void APlayerPawn::EndInvincibility()
{
	bIsInvincible = false;
	bBlinkVisible = true;

	GetWorldTimerManager().ClearTimer(BlinkTimerHandle);

	MeshComp->SetVisibility(true);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("HitIntensity"), 0.f);
	}
}

void APlayerPawn::BlinkMesh()
{
	bBlinkVisible = !bBlinkVisible;
	MeshComp->SetVisibility(bBlinkVisible);
}

void APlayerPawn::ReturnBulletToPool(ABulletActor* Bullet)
{
	BulletPool.Add(Bullet);
}

void APlayerPawn::OnInputFirePressed(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &APlayerPawn::FireBullet, FireInterval, true);
	FireBullet();
}

void APlayerPawn::OnInputFireReleased(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}
