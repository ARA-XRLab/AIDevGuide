// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSEnemyFSMComponent.h"

#include "TPSEnemyAnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UTPSEnemyFSMComponent::UTPSEnemyFSMComponent()
{
	// 적 상태 머신은 감지와 이동 상태 갱신이 필요하므로 Tick을 활성화합니다.
	PrimaryComponentTick.bCanEverTick = true;

	// 상태 전환 전의 초기 상태입니다.
	CurrentState = ETPSEnemyState::None;

	// 에디터에서 조정할 기본 플레이어 감지 범위입니다.
	DetectionRange = 1000.0f;

	// 에디터에서 조정할 기본 적 이동 속도입니다.
	MoveSpeed = 300.0f;

	// 에디터에서 조정할 기본 공격 감지 반경입니다.
	AttackHitRadius = 150.0f;

	// 에디터에서 조정할 기본 공격 데미지입니다.
	AttackDamage = 10.0f;

	// 에디터에서 조정할 기본 공격 쿨다운입니다.
	AttackCooldown = 1.0f;

	// 에디터에서 조정할 기본 피격 경직 시간입니다.
	HitReactDuration = 0.35f;

	// 에디터에서 조정할 기본 가라앉기 대기 시간입니다.
	sinkDelayTime = 3.0f;

	// 에디터에서 조정할 기본 가라앉기 속도입니다.
	sinkSpeed = 60.0f;

	// 에디터에서 조정할 기본 제거 기준 Z 위치입니다.
	removeZ = -200.0f;

	// 공격 가능한 초기 상태로 시작합니다.
	AttackCooldownRemaining = 0.0f;

	// 피격 전에는 남아 있는 경직 시간이 없습니다.
	HitReactTimeRemaining = 0.0f;

	// 사망 전에는 가라앉기 대기 시간이 없습니다.
	SinkDelayTimeRemaining = 0.0f;

	// 게임 시작 전에는 애니메이션 인스턴스 참조가 없습니다.
	Anim = nullptr;
}

void UTPSEnemyFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// 소유 액터가 Character일 때만 이동 처리를 수행합니다.
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		Anim = Cast<UTPSEnemyAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->AnimState = ETPSEnemyState::Idle;
			Anim->bAttackPlay = false;
		}
	}

	CachePlayerPawn();
	ChangeState(ETPSEnemyState::Idle);
}

void UTPSEnemyFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentState == ETPSEnemyState::Dead)
	{
		UpdateDeadState(DeltaTime);
		return;
	}

	if (CurrentState == ETPSEnemyState::Hit)
	{
		UpdateHitState(DeltaTime);
		return;
	}

	UpdateAttackCooldown(DeltaTime);

	switch (CurrentState)
	{
	case ETPSEnemyState::Idle:
		UpdateIdleState();
		break;

	case ETPSEnemyState::Move:
		if (UpdateMoveState())
		{
			MoveToPlayer();
		}
		break;

	case ETPSEnemyState::Attack:
		UpdateAttackState();
		break;

	default:
		break;
	}
}

void UTPSEnemyFSMComponent::NotifyHit()
{
	if (CurrentState == ETPSEnemyState::Dead)
	{
		return;
	}

	// 피격 요청이 들어오면 경직 시간을 새로 설정하고 즉시 피격 상태로 전환합니다.
	HitReactTimeRemaining = HitReactDuration;
	ChangeState(ETPSEnemyState::Hit);

	if (!OwnerCharacter)
	{
		return;
	}

	// 피격 경직 중에는 현재 이동 속도를 즉시 제거합니다.
	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
	}
}

void UTPSEnemyFSMComponent::NotifyDead()
{
	if (CurrentState == ETPSEnemyState::Dead)
	{
		return;
	}

	// 사망 상태에서는 이전 피격 경직을 더 이상 처리하지 않습니다.
	HitReactTimeRemaining = 0.0f;
	SinkDelayTimeRemaining = sinkDelayTime;
	ChangeState(ETPSEnemyState::Dead);
}

void UTPSEnemyFSMComponent::UpdateIdleState()
{
	if (!TargetPlayer)
	{
		CachePlayerPawn();
	}

	// 대기 상태에서는 플레이어가 감지 범위 안에 들어왔는지만 확인합니다.
	if (IsPlayerInDetectionRange())
	{
		ChangeState(ETPSEnemyState::Move);
	}
}

bool UTPSEnemyFSMComponent::UpdateMoveState()
{
	// 이동 상태에서는 플레이어가 감지 범위를 벗어나면 대기 상태로 복귀합니다.
	if (!IsPlayerInDetectionRange())
	{
		ChangeState(ETPSEnemyState::Idle);
		return false;
	}

	// 플레이어가 공격 범위 안에 있으면 공격 상태로 전환해 즉시 공격을 시작합니다.
	if (IsPlayerInAttackRange())
	{
		ChangeState(ETPSEnemyState::Attack);
		return false;
	}

	return true;
}

void UTPSEnemyFSMComponent::EnterAttackState()
{
	if (!OwnerCharacter)
	{
		ChangeState(ETPSEnemyState::Idle);
		return;
	}

	// 공격 상태에 진입하는 순간 이동을 즉시 멈춥니다.
	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
	}

	SetAttackPlay(true);
	AttackCooldownRemaining = AttackCooldown;

	ACharacter* AttackTarget = FindAttackTarget();
	if (!AttackTarget)
	{
		return;
	}

	UGameplayStatics::ApplyDamage(AttackTarget, AttackDamage, OwnerCharacter->GetController(), OwnerCharacter, UDamageType::StaticClass());
}

void UTPSEnemyFSMComponent::UpdateAttackState()
{
	if (!IsPlayerInAttackRange())
	{
		SetAttackPlay(false);
		ChangeState(ETPSEnemyState::Move);
		return;
	}

	if (CanEnterAttackState())
	{
		EnterAttackState();
	}
}

void UTPSEnemyFSMComponent::EnterDeadState()
{
	if (OwnerCharacter)
	{
		if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
		{
			MovementComponent->StopMovementImmediately();
			MovementComponent->DisableMovement();
		}

		if (UCapsuleComponent* Capsule = OwnerCharacter->GetCapsuleComponent())
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Capsule->SetGenerateOverlapEvents(false);
		}

		if (USkeletalMeshComponent* MeshComponent = OwnerCharacter->GetMesh())
		{
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			MeshComponent->SetGenerateOverlapEvents(false);
		}
	}

	SetAttackPlay(false);

	if (Anim)
	{
		Anim->PlayDeadAnim();
	}
}

void UTPSEnemyFSMComponent::UpdateDeadState(float DeltaTime)
{
	if (!OwnerCharacter)
	{
		return;
	}

	if (SinkDelayTimeRemaining > 0.0f)
	{
		SinkDelayTimeRemaining = FMath::Max(0.0f, SinkDelayTimeRemaining - DeltaTime);
		return;
	}

	// 사망 연출이 끝난 뒤 매 프레임 아래로 내려가게 합니다.
	const FVector CurrentLocation = OwnerCharacter->GetActorLocation();
	const FVector NextLocation = CurrentLocation + FVector(0.0f, 0.0f, -sinkSpeed * DeltaTime);
	OwnerCharacter->SetActorLocation(NextLocation, false);

	if (OwnerCharacter->GetActorLocation().Z <= removeZ)
	{
		OwnerCharacter->Destroy();
	}
}

void UTPSEnemyFSMComponent::UpdateHitState(float DeltaTime)
{
	// 피격 상태가 유지되는 동안에는 이동과 공격 상태 갱신을 수행하지 않습니다.
	HitReactTimeRemaining = FMath::Max(0.0f, HitReactTimeRemaining - DeltaTime);
	if (HitReactTimeRemaining > 0.0f)
	{
		return;
	}

	if (!TargetPlayer)
	{
		CachePlayerPawn();
	}

	// 경직이 끝나면 플레이어 유효성에 따라 추적 또는 대기 상태로 복귀합니다.
	ChangeState(TargetPlayer ? ETPSEnemyState::Move : ETPSEnemyState::Idle);
}

void UTPSEnemyFSMComponent::UpdateAttackCooldown(float DeltaTime)
{
	if (AttackCooldownRemaining <= 0.0f)
	{
		return;
	}

	AttackCooldownRemaining = FMath::Max(0.0f, AttackCooldownRemaining - DeltaTime);
}

void UTPSEnemyFSMComponent::CachePlayerPawn()
{
	// 현재 월드의 0번 플레이어 폰을 감지 대상으로 저장합니다.
	TargetPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
}

bool UTPSEnemyFSMComponent::IsPlayerInDetectionRange() const
{
	if (!OwnerCharacter || !TargetPlayer)
	{
		return false;
	}

	// 위아래 차이는 제외하고 수평 거리만 감지에 사용합니다.
	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	const FVector TargetLocation = TargetPlayer->GetActorLocation();
	const FVector HorizontalOffset = FVector(TargetLocation.X - OwnerLocation.X, TargetLocation.Y - OwnerLocation.Y, 0.0f);

	return HorizontalOffset.Size() <= DetectionRange;
}

bool UTPSEnemyFSMComponent::CanEnterAttackState() const
{
	return AttackCooldownRemaining <= 0.0f;
}

bool UTPSEnemyFSMComponent::IsPlayerInAttackRange() const
{
	if (!OwnerCharacter || !TargetPlayer)
	{
		return false;
	}

	// 위아래 차이는 제외하고 수평 거리만 공격 범위 판정에 사용합니다.
	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	const FVector TargetLocation = TargetPlayer->GetActorLocation();
	const FVector HorizontalOffset = FVector(TargetLocation.X - OwnerLocation.X, TargetLocation.Y - OwnerLocation.Y, 0.0f);

	return HorizontalOffset.Size() <= AttackHitRadius;
}

ACharacter* UTPSEnemyFSMComponent::FindAttackTarget() const
{
	if (!OwnerCharacter || !TargetPlayer || !GetWorld())
	{
		return nullptr;
	}

	ACharacter* TargetCharacter = Cast<ACharacter>(TargetPlayer);
	if (!TargetCharacter)
	{
		return nullptr;
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EnemyAttack), false, OwnerCharacter);
	QueryParams.AddIgnoredActor(OwnerCharacter);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	const bool bHasOverlap = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		OwnerCharacter->GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(AttackHitRadius),
		QueryParams);

	if (!bHasOverlap)
	{
		return nullptr;
	}

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (OverlapResult.GetActor() == TargetCharacter)
		{
			return TargetCharacter;
		}
	}

	return nullptr;
}

void UTPSEnemyFSMComponent::SetAttackPlay(bool bNewAttackPlay)
{
	if (Anim)
	{
		Anim->bAttackPlay = bNewAttackPlay;
	}
}

void UTPSEnemyFSMComponent::MoveToPlayer()
{
	if (!OwnerCharacter || !TargetPlayer)
	{
		CachePlayerPawn();
		return;
	}

	// 플레이어 방향에서 위아래 방향은 제거하고 수평 방향만 사용합니다.
	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	const FVector TargetLocation = TargetPlayer->GetActorLocation();
	const FVector MoveOffset = FVector(TargetLocation.X - OwnerLocation.X, TargetLocation.Y - OwnerLocation.Y, 0.0f);

	if (MoveOffset.IsNearlyZero())
	{
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = MoveSpeed;
	}

	const FVector MoveDirection = MoveOffset.GetSafeNormal();
	OwnerCharacter->AddMovementInput(MoveDirection, 1.0f, true);
}

void UTPSEnemyFSMComponent::ChangeState(ETPSEnemyState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	const ETPSEnemyState PreviousState = CurrentState;
	CurrentState = NewState;
	UE_LOG(LogTemp, Warning, TEXT("Enemy FSM State: %s"), GetStateName(CurrentState));

	if (Anim)
	{
		Anim->AnimState = CurrentState;
	}

	if (PreviousState == ETPSEnemyState::Attack && CurrentState != ETPSEnemyState::Attack)
	{
		SetAttackPlay(false);
	}

	if (CurrentState == ETPSEnemyState::Attack)
	{
		EnterAttackState();
	}
	else if (CurrentState == ETPSEnemyState::Dead)
	{
		EnterDeadState();
	}
}

const TCHAR* UTPSEnemyFSMComponent::GetStateName(ETPSEnemyState State) const
{
	switch (State)
	{
	case ETPSEnemyState::None:
		return TEXT("None");

	case ETPSEnemyState::Idle:
		return TEXT("Idle");

	case ETPSEnemyState::Move:
		return TEXT("Move");

	case ETPSEnemyState::Attack:
		return TEXT("Attack");

	case ETPSEnemyState::Hit:
		return TEXT("Hit");

	case ETPSEnemyState::Dead:
		return TEXT("Dead");

	default:
		return TEXT("Unknown");
	}
}
