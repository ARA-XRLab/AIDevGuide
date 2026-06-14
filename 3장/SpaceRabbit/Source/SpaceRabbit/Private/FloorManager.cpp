#include "FloorManager.h"

#include "Coin.h"
#include "Engine/World.h"
#include "FloorTile.h"
#include "Obstacle.h"
#include "TimerManager.h"

AFloorManager::AFloorManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CoinClass = ACoin::StaticClass();
}

void AFloorManager::BeginPlay()
{
	Super::BeginPlay();

	CreateTilePool();
	SpawnInitialTiles();
	ScheduleTileSpawn();
}

void AFloorManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void AFloorManager::CreateTilePool()
{
	if (FloorTileClass == nullptr || GetWorld() == nullptr || TilePoolSize <= 0)
	{
		return;
	}

	const FVector ManagerLocation = GetActorLocation();

	for (int32 TileIndex = 0; TileIndex < TilePoolSize; ++TileIndex)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AFloorTile* SpawnedTile = GetWorld()->SpawnActor<AFloorTile>(
			FloorTileClass,
			ManagerLocation,
			GetActorRotation(),
			SpawnParameters);

		if (SpawnedTile == nullptr)
		{
			continue;
		}

		BindTileExited(SpawnedTile);
		SpawnedTile->SetTileActive(false);
		ManagedTiles.Add(SpawnedTile);
		AvailableTiles.Add(SpawnedTile);
	}
}

void AFloorManager::SpawnInitialTiles()
{
	if (ManagedTiles.Num() == 0 || InitialTileCount <= 0)
	{
		return;
	}

	float CurrentSpawnY = InitialSpawnY;

	for (int32 TileIndex = 0; TileIndex < InitialTileCount; ++TileIndex)
	{
		AFloorTile* FloorTile = AcquireAvailableTile();
		if (FloorTile == nullptr)
		{
			break;
		}

		ActivateTile(FloorTile, CurrentSpawnY, false);
		CurrentSpawnY += FloorTile->GetTileWidth();
	}
}

void AFloorManager::BindTileExited(AFloorTile* FloorTile)
{
	if (FloorTile == nullptr)
	{
		return;
	}

	if (!FloorTile->OnTileExited.IsAlreadyBound(this, &AFloorManager::HandleTileExited))
	{
		FloorTile->OnTileExited.AddDynamic(this, &AFloorManager::HandleTileExited);
	}
}

void AFloorManager::ScheduleTileSpawn()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&AFloorManager::HandleSpawnTimerElapsed,
			SpawnInterval,
			true);
	}
}

AFloorTile* AFloorManager::AcquireAvailableTile()
{
	while (AvailableTiles.Num() > 0)
	{
		AFloorTile* FloorTile = AvailableTiles[0].Get();
		AvailableTiles.RemoveAt(0);

		if (FloorTile != nullptr)
		{
			return FloorTile;
		}
	}

	return nullptr;
}

AFloorTile* AFloorManager::GetLastSpawnedTile() const
{
	AFloorTile* LastSpawnedTile = nullptr;
	float MaxY = TNumericLimits<float>::Lowest();

	for (const TObjectPtr<AFloorTile>& FloorTile : ManagedTiles)
	{
		if (FloorTile == nullptr || FloorTile->IsHidden())
		{
			continue;
		}

		const float TileY = FloorTile->GetActorLocation().Y;
		if (TileY > MaxY)
		{
			MaxY = TileY;
			LastSpawnedTile = FloorTile;
		}
	}

	return LastSpawnedTile;
}

float AFloorManager::GetNextSpawnY() const
{
	const AFloorTile* LastSpawnedTile = GetLastSpawnedTile();
	if (LastSpawnedTile == nullptr)
	{
		return InitialSpawnY;
	}

	return LastSpawnedTile->GetActorLocation().Y + LastSpawnedTile->GetTileWidth();
}

float AFloorManager::GetGapOffset()
{
	if (RemainingGapCooldown > 0)
	{
		--RemainingGapCooldown;
		return 0.0f;
	}

	const bool bShouldCreateGap = FMath::FRand() <= GapChance;
	if (!bShouldCreateGap)
	{
		return 0.0f;
	}

	RemainingGapCooldown = GapCooldownSpawnCount;
	return GapSize;
}

void AFloorManager::DeactivateTile(AFloorTile* FloorTile)
{
	if (FloorTile == nullptr)
	{
		return;
	}

	ClearAttachedActors(FloorTile);
	FloorTile->SetTileActive(false);
	AvailableTiles.AddUnique(FloorTile);
}

void AFloorManager::ActivateTile(AFloorTile* FloorTile, float SpawnY, bool bPopulateContent)
{
	if (FloorTile == nullptr)
	{
		return;
	}

	ClearAttachedActors(FloorTile);

	const FVector ManagerLocation = GetActorLocation();
	FloorTile->SetActorLocation(FVector(ManagerLocation.X, SpawnY, ManagerLocation.Z));
	FloorTile->SetTileActive(true);

	if (bPopulateContent)
	{
		PopulateTile(FloorTile);
	}
}

void AFloorManager::ClearAttachedActors(AFloorTile* FloorTile) const
{
	if (FloorTile == nullptr)
	{
		return;
	}

	TArray<AActor*> AttachedActors;
	FloorTile->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (AttachedActor == nullptr)
		{
			continue;
		}

		if (AttachedActor->IsA(AObstacle::StaticClass()) || AttachedActor->IsA(ACoin::StaticClass()))
		{
			AttachedActor->Destroy();
		}
	}
}

void AFloorManager::PopulateTile(AFloorTile* FloorTile)
{
	if (SpawnObstacleOnTile(FloorTile))
	{
		return;
	}

	const bool bShouldSpawnCoins = FMath::FRand() <= CoinSpawnChance;
	if (!bShouldSpawnCoins)
	{
		return;
	}

	SpawnCoinsOnTile(FloorTile);
}

bool AFloorManager::SpawnObstacleOnTile(AFloorTile* FloorTile)
{
	if (FloorTile == nullptr || ObstacleClass == nullptr || GetWorld() == nullptr)
	{
		return false;
	}

	const bool bShouldSpawnObstacle = FMath::FRand() <= ObstacleSpawnChance;
	if (!bShouldSpawnObstacle)
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector ObstacleLocation = FloorTile->GetTopCenterLocation() + FVector(0.0f, 0.0f, ObstacleZOffset);

	AObstacle* SpawnedObstacle = GetWorld()->SpawnActor<AObstacle>(
		ObstacleClass,
		ObstacleLocation,
		FRotator::ZeroRotator,
		SpawnParameters);

	if (SpawnedObstacle == nullptr)
	{
		return false;
	}

	SpawnedObstacle->AttachToActor(FloorTile, FAttachmentTransformRules::KeepWorldTransform);
	return true;
}

void AFloorManager::SpawnCoinsOnTile(AFloorTile* FloorTile)
{
	if (FloorTile == nullptr || CoinClass == nullptr || GetWorld() == nullptr)
	{
		return;
	}

	const FVector TileLocation = FloorTile->GetActorLocation();
	const float TileWidth = FloorTile->GetTileWidth();
	const float CoinSpacing = FMath::Max(TileWidth * 0.25f, 80.0f);
	const float FirstCoinOffsetY = -CoinSpacing * 0.5f;
	const float SecondCoinOffsetY = CoinSpacing * 0.5f;
	const TArray<float, TInlineAllocator<2>> CoinOffsetsY = { FirstCoinOffsetY, SecondCoinOffsetY };

	for (const float CoinOffsetY : CoinOffsetsY)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const FVector CoinLocation(TileLocation.X, TileLocation.Y + CoinOffsetY, TileLocation.Z + CoinZOffset);
		ACoin* SpawnedCoin = GetWorld()->SpawnActor<ACoin>(
			CoinClass,
			CoinLocation,
			FRotator::ZeroRotator,
			SpawnParameters);

		if (SpawnedCoin == nullptr)
		{
			continue;
		}

		SpawnedCoin->AttachToActor(FloorTile, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AFloorManager::HandleTileExited(AFloorTile* FloorTile)
{
	DeactivateTile(FloorTile);
}

void AFloorManager::HandleSpawnTimerElapsed()
{
	AFloorTile* FloorTile = AcquireAvailableTile();
	if (FloorTile == nullptr)
	{
		return;
	}

	const float SpawnY = GetNextSpawnY() + GetGapOffset();
	ActivateTile(FloorTile, SpawnY, true);
}
