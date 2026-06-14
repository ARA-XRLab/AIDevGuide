#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "FloorManager.generated.h"

class AFloorTile;
class ACoin;
class AObstacle;

UCLASS()
class SPACERABBIT_API AFloorManager : public AActor
{
	GENERATED_BODY()

public:
	AFloorManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorManager")
	TSubclassOf<AFloorTile> FloorTileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorManager")
	int32 InitialTileCount = 6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorManager")
	int32 TilePoolSize = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorManager")
	float InitialSpawnY = -1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorManager")
	float SpawnInterval = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorManager")
	float GapChance = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorManager")
	float GapSize = 250.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorManager")
	int32 GapCooldownSpawnCount = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	TSubclassOf<AObstacle> ObstacleClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	float ObstacleSpawnChance = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	float ObstacleZOffset = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coin")
	TSubclassOf<ACoin> CoinClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coin")
	float CoinSpawnChance = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coin")
	float CoinZOffset = 100.0f;

private:
	UPROPERTY()
	TArray<TObjectPtr<AFloorTile>> ManagedTiles;

	UPROPERTY()
	TArray<TObjectPtr<AFloorTile>> AvailableTiles;

	FTimerHandle SpawnTimerHandle;
	int32 RemainingGapCooldown = 0;

	void CreateTilePool();
	void SpawnInitialTiles();
	void BindTileExited(AFloorTile* FloorTile);
	void ScheduleTileSpawn();
	AFloorTile* AcquireAvailableTile();
	AFloorTile* GetLastSpawnedTile() const;
	float GetNextSpawnY() const;
	float GetGapOffset();
	void DeactivateTile(AFloorTile* FloorTile);
	void ActivateTile(AFloorTile* FloorTile, float SpawnY, bool bPopulateContent);
	void ClearAttachedActors(AFloorTile* FloorTile) const;
	void PopulateTile(AFloorTile* FloorTile);
	bool SpawnObstacleOnTile(AFloorTile* FloorTile);
	void SpawnCoinsOnTile(AFloorTile* FloorTile);

	UFUNCTION()
	void HandleTileExited(AFloorTile* FloorTile);

	UFUNCTION()
	void HandleSpawnTimerElapsed();
};
