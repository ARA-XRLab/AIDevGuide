// Fill out your copyright notice in the Description page of Project Settings.

#include "DestroyZoneActor.h"

#include "PlayerBulletActor.h"
#include "PlayerPawn.h"
#include "Components/BoxComponent.h"

ADestroyZoneActor::ADestroyZoneActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(50.f));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionProfileName(TEXT("DestroyZone"));

	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

void ADestroyZoneActor::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ADestroyZoneActor::OnBoxCompOverlap);
}

void ADestroyZoneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADestroyZoneActor::OnBoxCompOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (ABulletActor* Bullet = Cast<ABulletActor>(OtherActor))
	{
		Bullet->SetActive(false);

		if (APlayerPawn* Player = Cast<APlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			Player->ReturnBulletToPool(Bullet);
		}
	}
	else
	{
		OtherActor->Destroy();
	}
}
