#include "FloorTile.h"

#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"

AFloorTile::AFloorTile()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BoxComponent->SetGenerateOverlapEvents(false);

	MeshComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(BoxComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UpdateTileSize();
}

void AFloorTile::BeginPlay()
{
	Super::BeginPlay();

	UpdateTileSize();
}

void AFloorTile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MoveTile(DeltaSeconds);
	UpdateExitState();
}

FVector AFloorTile::GetTopCenterLocation() const
{
	if (BoxComponent == nullptr)
	{
		return GetActorLocation();
	}

	const FVector BoxExtent = BoxComponent->GetScaledBoxExtent();
	return BoxComponent->GetComponentLocation() + FVector(0.0f, 0.0f, BoxExtent.Z);
}

void AFloorTile::SetTileActive(bool bInIsActive)
{
	bHasExited = false;
	SetActorHiddenInGame(!bInIsActive);
	SetActorEnableCollision(bInIsActive);
	SetActorTickEnabled(bInIsActive);
}

void AFloorTile::UpdateTileSize()
{
	const FVector BoxExtent(50.0f, TileWidth * 0.5f, 50.0f);
	BoxComponent->SetBoxExtent(BoxExtent);

	// 기본 타일 폭 500 기준으로 스프라이트 길이를 X축으로 보정한다.
	constexpr float BaseTileWidth = 500.0f;
	constexpr float BaseSpriteScaleX = 0.25f;
	constexpr float BaseSpriteScaleY = 1.0f;
	constexpr float BaseSpriteScaleZ = 0.4f;

	const float WidthScaleRatio = TileWidth / BaseTileWidth;
	MeshComponent->SetRelativeScale3D(FVector(BaseSpriteScaleX * WidthScaleRatio, BaseSpriteScaleY, BaseSpriteScaleZ));
}

void AFloorTile::MoveTile(float DeltaSeconds)
{
	AddActorWorldOffset(FVector(0.0f, -(MoveSpeed * DeltaSeconds), 0.0f), false);
}

void AFloorTile::UpdateExitState()
{
	const bool bIsPastDestroyLine = GetActorLocation().Y <= DestroyY;

	if (bIsPastDestroyLine && !bHasExited)
	{
		bHasExited = true;
		OnTileExited.Broadcast(this);
		return;
	}

	if (!bIsPastDestroyLine)
	{
		bHasExited = false;
	}
}
