// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBulletActor.h"

#include "EnemyActor.h"
#include "PlayerPawn.h"
#include "ShootingGameMode.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

ABulletActor::ABulletActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(37.5f, 12.5f, 50.f));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetWorldScale3D(FVector(0.75f, 0.25f, 1.f));

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (MeshFinder.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshFinder.Object);
	}
	ConstructorHelpers::FObjectFinder<UMaterialInstance> MaterialFinder(TEXT("/Game/Shooting/Materials/MI_BulletBlue.MI_BulletBlue"));
	if (MaterialFinder.Succeeded())
	{
		MeshComp->SetMaterial(0, MaterialFinder.Object);
	}

	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionProfileName(TEXT("PlayerBullet"));

	ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionVFXFinder(TEXT("/Game/Shooting/Particles/P_Explosion.P_Explosion"));
	if (ExplosionVFXFinder.Succeeded())
	{
		ExplosionVFX = ExplosionVFXFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundCue> ExplosionSoundFinder(TEXT("/Game/Shooting/Sound/Explosion.Explosion"));
	if (ExplosionSoundFinder.Succeeded())
	{
		ExplosionSound = ExplosionSoundFinder.Object;
	}
}

void ABulletActor::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABulletActor::OnBoxCompOverlap);
}

void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MeshComp->IsVisible())
	{
		return;
	}

	FVector Direction = GetActorForwardVector();
	SetActorLocation(GetActorLocation() + Direction * Speed * DeltaTime);
}

void ABulletActor::OnBoxCompOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	AEnemyActor* Enemy = Cast<AEnemyActor>(OtherActor);
	if (Enemy)
	{
		if (AShootingGameMode* GameMode = Cast<AShootingGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->AddScore(100);
		}
		Enemy->Destroy();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation());
		UGameplayStatics::PlaySound2D(GetWorld(), ExplosionSound);

		SetActive(false);

		if (APlayerPawn* Player = Cast<APlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			Player->ReturnBulletToPool(this);
		}
	}
}

void ABulletActor::SetActive(bool bActive)
{
	MeshComp->SetVisibility(bActive);
	BoxComp->SetCollisionEnabled(bActive ? ECollisionEnabled::Type::QueryAndPhysics : ECollisionEnabled::Type::NoCollision);
}