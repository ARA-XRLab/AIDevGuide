// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyActor.h"

#include "EnemyBulletActor.h"
#include "PlayerPawn.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

AEnemyActor::AEnemyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	BoxComp->SetBoxExtent(FVector(50.f));
	MeshComp->SetRelativeLocation(FVector(0, 0, -60.f));
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Shooting/Models/SphericalRobot/SphericalRobot.SphericalRobot"));
	if (MeshFinder.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshFinder.Object);
	}
	ConstructorHelpers::FObjectFinder<UMaterialInstance> MaterialFinder(TEXT("/Game/Shooting/Models/SphericalRobot/MI_SphericalRobot.MI_SphericalRobot"));
	if (MaterialFinder.Succeeded())
	{
		MeshComp->SetMaterial(0, MaterialFinder.Object);
	}

	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionProfileName(TEXT("Enemy"));

	ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionVFXFinder(TEXT("/Game/Shooting/Particles/P_Explosion.P_Explosion"));
	if (ExplosionVFXFinder.Succeeded())
	{
		ExplosionVFX = ExplosionVFXFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundCue> FireSoundFinder(TEXT("/Game/Shooting/Sound/EnemyBullet.EnemyBullet"));
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

void AEnemyActor::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyActor::OnBoxCompOverlap);

	APawn* TargetPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if ( TargetPawn != nullptr )
	{
		MoveDirection = TargetPawn->GetActorLocation() - GetActorLocation();
		MoveDirection.Normalize();
	}

	FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(MoveDirection, GetActorUpVector());
	SetActorRotation(NewRotation);
}

void AEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 발사 중이면 이동하지 않음
	if (!bIsFiring)
	{
		SetActorLocation(GetActorLocation() + MoveDirection * Speed * DeltaTime);
		TryStartFiring();
	}
}

void AEnemyActor::TryStartFiring()
{
	APawn* PlayerPawn = GetWorld() ? GetWorld()->GetFirstPlayerController()->GetPawn() : nullptr;
	if (!PlayerPawn)
	{
		return;
	}

	const FVector MyLoc = GetActorLocation();
	const FVector PlayerLoc = PlayerPawn->GetActorLocation();

	// 거리 체크
	const float Distance = FVector::Dist(MyLoc, PlayerLoc);
	if (Distance > EncounterRadius)
	{
		return;
	}

	// 플레이어 방향 계산 (Y-Z 평면)
	FireDirection = PlayerLoc - MyLoc;
	FireDirection.X = 0.f;

	if (FireDirection.IsNearlyZero())
	{
		return;
	}
	FireDirection.Normalize();

	// 발사 시작
	bIsFiring = true;
	TargetBurstCount = FMath::RandRange(MinBurstCount, MaxBurstCount);
	CurrentBurstCount = TargetBurstCount;

	// 플레이어 방향으로 회전
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(FireDirection, GetActorUpVector());
	SetActorRotation(NewRotation);

	// 첫 발 즉시 발사 후 타이머로 연속 발사
	FireOneBullet();
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyActor::FireOneBullet, FireInterval, true);
}

void AEnemyActor::FireOneBullet()
{
	if (!EnemyBulletClass)
	{
		EndFiring();
		return;
	}

	CurrentBurstCount++;

	const FRotator FireRot = FireDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	AEnemyBulletActor* Bullet = GetWorld()->SpawnActor<AEnemyBulletActor>(
		EnemyBulletClass,
		GetActorLocation(),
		FireRot,
		SpawnParams
	);

	if (Bullet)
	{
		Bullet->SetActive(true);
		UGameplayStatics::PlaySound2D(GetWorld(), FireSound);
	}

	// 연발 완료 체크
	if (CurrentBurstCount >= TargetBurstCount)
	{
		EndFiring();
	}
}

void AEnemyActor::EndFiring()
{
	bIsFiring = false;
	bHasFired = true;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);

	// 발사 방향으로 이동 방향 변경
	MoveDirection = FireDirection;
}

void AEnemyActor::OnBoxCompOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerPawn* Player = Cast<APlayerPawn>(OtherActor);
	if (Player)
	{
		if (Player->IsInvincible())
		{
			return;
		}

		Player->OnDamage(1);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation());
		UGameplayStatics::PlaySound2D(GetWorld(), ExplosionSound);

		Destroy();
	}
}
