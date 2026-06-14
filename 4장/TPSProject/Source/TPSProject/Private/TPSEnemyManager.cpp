// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSEnemyManager.h"

#include "TPSEnemyCharacter.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ATPSEnemyManager::ATPSEnemyManager()
{
	// 적 매니저는 타이머만 사용하므로 매 프레임 Tick을 비활성화합니다.
	PrimaryActorTick.bCanEverTick = false;

	// 적 클래스는 에디터에서 지정합니다.
	EnemyClass = nullptr;

	// 에디터에서 조정할 기본 스폰 주기입니다.
	SpawnInterval = 3.0f;

	// 에디터에서 조정할 기본 최대 동시 존재 수입니다.
	MaxEnemyCount = 5;

	// 에디터에서 스폰 위치 액터에 부여할 기본 태그 이름입니다.
	SpawnPointTag = TEXT("EnemySpawn");
}

void ATPSEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	CollectSpawnPoints();
	StartSpawnTimerIfReady();
}

void ATPSEnemyManager::CollectSpawnPoints()
{
	SpawnPoints.Reset();

	if (SpawnPointTag.IsNone())
	{
		return;
	}

	TArray<AActor*> FoundSpawnPoints;
	UGameplayStatics::GetAllActorsWithTag(this, SpawnPointTag, FoundSpawnPoints);

	for (AActor* SpawnPoint : FoundSpawnPoints)
	{
		if (SpawnPoint)
		{
			SpawnPoints.Add(SpawnPoint);
		}
	}
}

void ATPSEnemyManager::StartSpawnTimerIfReady()
{
	if (!GetWorld() || !EnemyClass || SpawnInterval <= 0.0f || MaxEnemyCount <= 0 || SpawnPoints.IsEmpty())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ATPSEnemyManager::SpawnEnemy, SpawnInterval, true);
}

void ATPSEnemyManager::SpawnEnemy()
{
	if (!GetWorld() || !EnemyClass)
	{
		return;
	}

	RemoveInvalidEnemies();
	if (SpawnedEnemies.Num() >= MaxEnemyCount)
	{
		return;
	}

	AActor* SpawnPoint = GetRandomSpawnPoint();
	if (!SpawnPoint)
	{
		CollectSpawnPoints();
		SpawnPoint = GetRandomSpawnPoint();
	}

	if (!SpawnPoint)
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ATPSEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<ATPSEnemyCharacter>(
		EnemyClass,
		SpawnPoint->GetActorLocation(),
		SpawnPoint->GetActorRotation(),
		SpawnParameters);

	if (SpawnedEnemy)
	{
		SpawnedEnemies.Add(SpawnedEnemy);
	}
}

void ATPSEnemyManager::RemoveInvalidEnemies()
{
	SpawnedEnemies.RemoveAll([](const TWeakObjectPtr<ATPSEnemyCharacter>& SpawnedEnemy)
	{
		return !SpawnedEnemy.IsValid();
	});
}

AActor* ATPSEnemyManager::GetRandomSpawnPoint() const
{
	TArray<AActor*> ValidSpawnPoints;
	for (const TWeakObjectPtr<AActor>& SpawnPoint : SpawnPoints)
	{
		if (SpawnPoint.IsValid())
		{
			ValidSpawnPoints.Add(SpawnPoint.Get());
		}
	}

	if (ValidSpawnPoints.IsEmpty())
	{
		return nullptr;
	}

	const int32 RandomIndex = FMath::RandRange(0, ValidSpawnPoints.Num() - 1);
	return ValidSpawnPoints[RandomIndex];
}
