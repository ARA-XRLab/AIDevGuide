// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBulletActor.h"

#include "PlayerPawn.h"
#include "Components/BoxComponent.h"

AEnemyBulletActor::AEnemyBulletActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(10.f, 10.f, 10.f));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetWorldScale3D(FVector(0.25f));

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (MeshFinder.Succeeded())
	{
		MeshComp->SetStaticMesh(MeshFinder.Object);
	}

	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/Shooting/Materials/MI_BulletRed.MI_BulletRed"));
	if (MaterialFinder.Succeeded())
	{
		MeshComp->SetMaterial(0, MaterialFinder.Object);
	}

	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionProfileName(TEXT("EnemyBullet"));
}

void AEnemyBulletActor::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBulletActor::OnBoxCompOverlap);
}

void AEnemyBulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActive)
	{
		return;
	}

	FVector Direction = GetActorForwardVector();
	SetActorLocation(GetActorLocation() + Direction * Speed * DeltaTime);
}

void AEnemyBulletActor::OnBoxCompOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	APlayerPawn* Player = Cast<APlayerPawn>(OtherActor);
	if (Player)
	{
		if (!Player->IsInvincible())
		{
			Player->OnDamage(Damage);
		}

		SetActive(false);
	}
}

void AEnemyBulletActor::SetActive(bool bActive)
{
	bIsActive = bActive;
	MeshComp->SetVisibility(bActive);
	BoxComp->SetCollisionEnabled(bActive ? ECollisionEnabled::Type::QueryAndPhysics : ECollisionEnabled::Type::NoCollision);

	if (bActive)
	{
	}
	else
	{
		GetWorldTimerManager().ClearTimer(LifeTimeTimerHandle);
	}
}

void AEnemyBulletActor::ResetBullet()
{
	SetActive(false);
	SetActorLocation(FVector::ZeroVector);
	SetActorRotation(FRotator::ZeroRotator);
}

bool AEnemyBulletActor::IsActive() const
{
	return bIsActive;
}

void AEnemyBulletActor::OnLifeTimeExpired()
{
	SetActive(false);
}
