#include "RocketSpawner.h"

#include "Engine/World.h"
#include "Rocket.h"
#include "SpaceRabbitGameMode.h"
#include "TimerManager.h"

ARocketSpawner::ARocketSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARocketSpawner::BeginPlay()
{
	Super::BeginPlay();

	CachedGameMode = GetWorld() != nullptr ? GetWorld()->GetAuthGameMode<ASpaceRabbitGameMode>() : nullptr;
	if (CachedGameMode != nullptr)
	{
		CachedGameMode->OnGameOver.AddDynamic(this, &ARocketSpawner::HandleGameOver);

		if (CachedGameMode->IsGameOver())
		{
			return;
		}
	}

	ScheduleNextSpawn();
}

void ARocketSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CachedGameMode != nullptr)
	{
		CachedGameMode->OnGameOver.RemoveDynamic(this, &ARocketSpawner::HandleGameOver);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void ARocketSpawner::ScheduleNextSpawn()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (CachedGameMode != nullptr && CachedGameMode->IsGameOver())
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ARocketSpawner::SpawnRocket,
		GetRandomSpawnInterval(),
		false);
}

void ARocketSpawner::SpawnRocket()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (CachedGameMode != nullptr && CachedGameMode->IsGameOver())
	{
		return;
	}

	if (RocketClass != nullptr)
	{
		const FVector SpawnerLocation = GetActorLocation();
		const FVector SpawnLocation(
			SpawnerLocation.X,
			SpawnerLocation.Y,
			SpawnerLocation.Z + GetRandomSpawnZOffset());

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		World->SpawnActor<ARocket>(RocketClass, SpawnLocation, GetActorRotation(), SpawnParameters);
	}

	ScheduleNextSpawn();
}

float ARocketSpawner::GetRandomSpawnInterval() const
{
	const float ClampedMinInterval = FMath::Max(0.1f, MinSpawnInterval);
	const float ClampedMaxInterval = FMath::Max(ClampedMinInterval, MaxSpawnInterval);
	return FMath::FRandRange(ClampedMinInterval, ClampedMaxInterval);
}

float ARocketSpawner::GetRandomSpawnZOffset() const
{
	const float MinOffset = FMath::Min(MinSpawnZ, MaxSpawnZ);
	const float MaxOffset = FMath::Max(MinSpawnZ, MaxSpawnZ);
	return FMath::FRandRange(MinOffset, MaxOffset);
}

void ARocketSpawner::HandleGameOver()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}
}
