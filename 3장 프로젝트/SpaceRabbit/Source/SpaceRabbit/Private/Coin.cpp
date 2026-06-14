#include "Coin.h"

#include "Components/SphereComponent.h"
#include "PaperSpriteComponent.h"
#include "SpaceRabbitCharacter.h"

ACoin::ACoin()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->InitSphereRadius(32.0f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComponent->SetGenerateOverlapEvents(true);

	MeshComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetRelativeScale3D(FVector(0.64f));
}

void ACoin::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnSphereOverlap);
}

void ACoin::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (Cast<ASpaceRabbitCharacter>(OtherActor) == nullptr)
	{
		return;
	}

	OnCoinCollected.Broadcast(this, OtherActor);
	Destroy();
}
