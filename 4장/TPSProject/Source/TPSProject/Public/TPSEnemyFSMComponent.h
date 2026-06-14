// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSEnemyFSMComponent.generated.h"

class ACharacter;
class APawn;
class UTPSEnemyAnimInstance;

// 적 AI가 사용할 상태 목록입니다.
UENUM(BlueprintType)
enum class ETPSEnemyState : uint8
{
	None,
	Idle,
	Move,
	Attack,
	Hit,
	Dead
};

// 적 캐릭터의 단순 상태 전환과 직선 이동을 관리하는 컴포넌트입니다.
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TPSPROJECT_API UTPSEnemyFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 적 FSM 컴포넌트의 기본 설정을 초기화하는 생성자입니다.
	UTPSEnemyFSMComponent();

	// 매 프레임 현재 상태에 맞는 적 행동을 갱신하는 함수입니다.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 데미지 수신 시 즉시 피격 상태로 전환하고 경직 시간을 설정하는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "FSM|Hit")
	void NotifyHit();

	// 사망 시 즉시 사망 상태로 전환하고 사망 연출을 시작하는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "FSM|Dead")
	void NotifyDead();

protected:
	// 게임 시작 시 소유 캐릭터를 저장하고 초기 상태를 설정하는 함수입니다.
	virtual void BeginPlay() override;

	// 현재 적 AI 상태입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	ETPSEnemyState CurrentState;

	// 플레이어를 감지할 수 있는 거리입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DetectionRange;

	// 플레이어를 향해 이동할 때 사용할 이동 속도입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MoveSpeed;

	// 공격 상태에 진입하고 근접 공격을 실행할 수 있는 범위입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM|Attack", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AttackHitRadius;

	// 공격 대상에게 전달할 데미지 수치입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM|Attack", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AttackDamage;

	// 공격 상태에서 다음 근접 공격까지 기다릴 딜레이 시간입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM|Attack", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AttackCooldown;

	// 피격 상태에서 이동과 공격을 막는 경직 시간입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM|Hit", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float HitReactDuration;

	// 사망 후 가라앉기 시작 전까지 대기할 시간입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM|Dead", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float sinkDelayTime;

	// 사망 후 아래로 가라앉는 속도입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM|Dead", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float sinkSpeed;

	// 적이 이 Z 위치 이하로 내려가면 제거할 기준 높이입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM|Dead")
	float removeZ;

	// 현재 남아 있는 공격 쿨다운 시간입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM|Attack")
	float AttackCooldownRemaining;

	// 현재 남아 있는 피격 경직 시간입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM|Hit")
	float HitReactTimeRemaining;

	// 사망 후 가라앉기 시작 전까지 남은 시간입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM|Dead")
	float SinkDelayTimeRemaining;

	// 이 컴포넌트를 소유한 적 캐릭터입니다.
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwnerCharacter;

	// 감지 대상으로 사용할 플레이어 폰입니다.
	UPROPERTY(Transient)
	TObjectPtr<APawn> TargetPlayer;

	// 적 애니메이션 블루프린트로 FSM 상태를 전달할 내부 참조입니다.
	UPROPERTY(Transient)
	TObjectPtr<UTPSEnemyAnimInstance> Anim;

	// 대기 상태에서 플레이어 감지 여부를 확인하는 함수입니다.
	void UpdateIdleState();

	// 이동 상태에서 플레이어 추적과 감지 범위 이탈 여부를 처리하고 이동 가능 여부를 반환하는 함수입니다.
	bool UpdateMoveState();

	// 공격 상태에 진입할 때 이동 정지, 대상 감지, 데미지 적용을 처리하는 함수입니다.
	void EnterAttackState();

	// 공격 상태에서 범위 이탈과 반복 공격을 처리하는 함수입니다.
	void UpdateAttackState();

	// 사망 상태에 진입할 때 이동, 충돌, 사망 애니메이션을 처리하는 함수입니다.
	void EnterDeadState();

	// 사망 상태에서 대기 후 아래로 가라앉고 제거 여부를 확인하는 함수입니다.
	void UpdateDeadState(float DeltaTime);

	// 피격 상태에서 경직 시간을 갱신하고 다음 상태를 결정하는 함수입니다.
	void UpdateHitState(float DeltaTime);

	// 공격 쿨다운을 매 프레임 감소시키는 함수입니다.
	void UpdateAttackCooldown(float DeltaTime);

	// 플레이어 폰을 찾아 TargetPlayer에 저장하는 함수입니다.
	void CachePlayerPawn();

	// 플레이어가 감지 범위 안에 있는지 확인하는 함수입니다.
	bool IsPlayerInDetectionRange() const;

	// 공격 상태에 진입할 수 있는지 확인하는 함수입니다.
	bool CanEnterAttackState() const;

	// 플레이어가 공격 범위 안에 있는지 확인하는 함수입니다.
	bool IsPlayerInAttackRange() const;

	// 공격 반경 안에 있는 캐릭터 대상을 찾는 함수입니다.
	ACharacter* FindAttackTarget() const;

	// 공격 애니메이션 재생 상태를 애니메이션 인스턴스에 전달하는 함수입니다.
	void SetAttackPlay(bool bNewAttackPlay);

	// 플레이어를 향해 수평 방향으로 직선 이동시키는 함수입니다.
	void MoveToPlayer();

	// 상태를 변경하고 변경된 상태 이름을 로그로 출력하는 함수입니다.
	void ChangeState(ETPSEnemyState NewState);

	// 상태 값을 로그용 이름으로 변환하는 함수입니다.
	const TCHAR* GetStateName(ETPSEnemyState State) const;
};
