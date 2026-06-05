#include "Obstacle.h"

#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "SpaceRabbitCharacter.h"

AObstacle::AObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetBoxExtent(FVector(35.0f, 35.0f, 35.0f));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetGenerateOverlapEvents(true);

	MeshComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(BoxComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AObstacle::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AObstacle::OnBoxOverlap);
}

void AObstacle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetAttachParentActor() == nullptr)
	{
		MoveObstacle(DeltaSeconds);
	}

	UpdateDestroyState();
}

void AObstacle::OnBoxOverlap(
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

void AObstacle::MoveObstacle(float DeltaSeconds)
{
	AddActorWorldOffset(FVector(0.0f, -(MoveSpeed * DeltaSeconds), 0.0f), false);
}

void AObstacle::UpdateDestroyState()
{
	if (GetActorLocation().Y <= DestroyY)
	{
		Destroy();
	}
}
