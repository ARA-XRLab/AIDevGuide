#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketSpawner.generated.h"

class ARocket;
class ASpaceRabbitGameMode;

UCLASS()
class SPACERABBIT_API ARocketSpawner : public AActor
{
	GENERATED_BODY()

public:
	ARocketSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<ARocket> RocketClass;

	// Minimum delay before the next rocket spawn (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = "0.1"))
	float MinSpawnInterval = 3.0f;

	// Maximum delay before the next rocket spawn (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = "0.1"))
	float MaxSpawnInterval = 6.0f;

	// Minimum Z offset from the spawner location
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	float MinSpawnZ = -100.0f;

	// Maximum Z offset from the spawner location
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	float MaxSpawnZ = 100.0f;

private:
	void ScheduleNextSpawn();
	void SpawnRocket();
	float GetRandomSpawnInterval() const;
	float GetRandomSpawnZOffset() const;

	UFUNCTION()
	void HandleGameOver();

	UPROPERTY(Transient)
	TObjectPtr<ASpaceRabbitGameMode> CachedGameMode;

	FTimerHandle SpawnTimerHandle;
};
