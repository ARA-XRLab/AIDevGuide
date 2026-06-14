// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSEnemyCharacter.generated.h"

class UTPSEnemyFSMComponent;

// TPS 프로젝트에서 적으로 배치할 기본 캐릭터 클래스입니다.
UCLASS()
class TPSPROJECT_API ATPSEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 적 캐릭터의 기본 설정을 초기화하는 생성자입니다.
	ATPSEnemyCharacter();

	// 데미지를 받았을 때 호출되어 받은 데미지 수치를 처리하는 함수입니다.
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 에디터에서 변경된 메시 보정 값을 기본 메시 슬롯에 반영하는 함수입니다.
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// 게임 시작 시 스폰된 적도 컨트롤러 없이 이동할 수 있도록 이동 상태를 보정하는 함수입니다.
	virtual void BeginPlay() override;

	// 적 캐릭터의 상태 전환과 단순 추적 이동을 담당하는 FSM 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UTPSEnemyFSMComponent> EnemyFSMComponent;

	// 기본 스켈레탈 메시의 위치 보정 값입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FVector EnemyMeshRelativeLocation;

	// 기본 스켈레탈 메시의 회전 보정 값입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FRotator EnemyMeshRelativeRotation;

	// 이동 방향을 향해 회전할 때 사용할 회전 속도입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FRotator EnemyRotationRate;

	// 적 캐릭터 이동 컴포넌트의 최대 이동 속도입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float EnemyMaxWalkSpeed;

	// 현재 남아 있는 적 HP입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP", meta = (ClampMin = "0", UIMin = "0"))
	int32 hp;

	// 적 HP의 최대값입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP", meta = (ClampMin = "0", UIMin = "0"))
	int32 maxHp;

	// 사망 처리가 이미 실행되었는지 저장하는 값입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Death")
	bool bIsDead;

	// 적 캐릭터의 사망 상태 처리를 FSM에 요청하는 함수입니다.
	void HandleDead();
};
