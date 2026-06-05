// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSEnemyManager.generated.h"

class ATPSEnemyCharacter;

// 적 캐릭터를 지정한 태그 위치에서 주기적으로 생성하는 매니저 액터입니다.
UCLASS()
class TPSPROJECT_API ATPSEnemyManager : public AActor
{
	GENERATED_BODY()

public:
	// 적 매니저의 기본 스폰 설정을 초기화하는 생성자입니다.
	ATPSEnemyManager();

protected:
	// 게임 시작 시 스폰 위치를 수집하고 조건이 맞으면 스폰 타이머를 시작하는 함수입니다.
	virtual void BeginPlay() override;

	// 타이머에서 생성할 적 캐릭터 클래스입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawn")
	TSubclassOf<ATPSEnemyCharacter> EnemyClass;

	// 적을 생성할 주기입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawn", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float SpawnInterval;

	// 동시에 월드에 존재할 수 있는 최대 적 수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawn", meta = (ClampMin = "0", UIMin = "0"))
	int32 MaxEnemyCount;

	// 스폰 위치 액터를 찾을 때 사용할 태그 이름입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawn")
	FName SpawnPointTag;

	// 태그로 수집한 스폰 위치 액터 목록입니다.
	TArray<TWeakObjectPtr<AActor>> SpawnPoints;

	// 매니저가 생성한 적 캐릭터 목록입니다.
	TArray<TWeakObjectPtr<ATPSEnemyCharacter>> SpawnedEnemies;

	// 주기적 스폰을 실행하는 타이머 핸들입니다.
	FTimerHandle SpawnTimerHandle;

	// 태그를 가진 스폰 위치 액터를 월드에서 수집하는 함수입니다.
	void CollectSpawnPoints();

	// 스폰 조건이 유효할 때 타이머를 시작하는 함수입니다.
	void StartSpawnTimerIfReady();

	// 현재 적 수가 최대 수보다 적으면 랜덤 위치에 적을 생성하는 함수입니다.
	void SpawnEnemy();

	// 유효하지 않은 적 참조를 목록에서 제거하는 함수입니다.
	void RemoveInvalidEnemies();

	// 유효한 스폰 위치 중 하나를 랜덤으로 반환하는 함수입니다.
	AActor* GetRandomSpawnPoint() const;
};
