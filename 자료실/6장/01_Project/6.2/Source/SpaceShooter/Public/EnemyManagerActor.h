// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManagerActor.generated.h"

UCLASS()
class SPACESHOOTER_API AEnemyManagerActor : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyManagerActor();

	// Lifecycle
	virtual void Tick(float DeltaTime) override;

	// Runtime
	UPROPERTY(EditAnywhere)
	float SpawnFrequency = 2.0f;
	
protected:
	// Lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Internal
	void SpawnEnemy();

	// Components
	UPROPERTY(EditAnywhere)
	class USceneComponent* RootComp;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* SpawnPoint;

	// Config
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemyActor> EnemyFactory;

	// Runtime
	UPROPERTY(VisibleAnywhere)
	TArray<class AActor*> SpawnPoints;

private:
	// Internal State
	FTimerHandle SpawnTimerHandle;
	int32 PrevSpawnIndex = -1;
};
