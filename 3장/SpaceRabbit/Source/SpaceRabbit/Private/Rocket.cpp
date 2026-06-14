#include "Rocket.h"

#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "SpaceRabbitCharacter.h"

ARocket::ARocket()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetBoxExtent(FVector(60.0f, 30.0f, 30.0f));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetGenerateOverlapEvents(true);

	MeshComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(BoxComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetRelativeScale3D(FVector(1.2f, 0.6f, 0.6f));
}

void ARocket::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARocket::OnBoxOverlap);
}

void ARocket::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MoveRocket(DeltaSeconds);
	UpdateDestroyState();
}

void ARocket::OnBoxOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ASpaceRabbitCharacter* SpaceRabbitCharacter = Cast<ASpaceRabbitCharacter>(OtherActor);
	if (SpaceRabbitCharacter == nullptr)
	{
		return;
	}

	SpaceRabbitCharacter->TriggerGameOver();
	Destroy();
}

void ARocket::MoveRocket(float DeltaSeconds)
{
	AddActorWorldOffset(FVector(0.0f, -(RocketSpeed * DeltaSeconds), 0.0f), false);
}

void ARocket::UpdateDestroyState()
{
	if (GetActorLocation().Y <= DestroyY)
	{
		Destroy();
	}
}
