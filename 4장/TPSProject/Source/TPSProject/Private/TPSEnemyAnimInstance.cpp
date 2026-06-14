// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSEnemyAnimInstance.h"

#include "Animation/AnimMontage.h"

UTPSEnemyAnimInstance::UTPSEnemyAnimInstance()
{
	// 적 애니메이션은 대기 상태를 기본값으로 시작합니다.
	AnimState = ETPSEnemyState::Idle;

	// 시작 시에는 공격 애니메이션을 재생하지 않는 상태입니다.
	bAttackPlay = false;

	// 피격 몽타주는 에디터에서 연결합니다.
	DamageMontage = nullptr;

	// 사망 몽타주는 에디터에서 연결합니다.
	DeadMontage = nullptr;
}

void UTPSEnemyAnimInstance::AnimNotify_AttackEnd()
{
	// 공격 애니메이션 노티파이가 호출되면 공격 재생 상태를 끕니다.
	bAttackPlay = false;
}

void UTPSEnemyAnimInstance::PlayDamageAnim()
{
	if (!DamageMontage)
	{
		return;
	}

	Montage_Play(DamageMontage);
}

void UTPSEnemyAnimInstance::PlayDeadAnim()
{
	if (!DeadMontage)
	{
		return;
	}

	Montage_Play(DeadMontage);
}
