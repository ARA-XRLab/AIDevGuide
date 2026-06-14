// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSEnemyFSMComponent.h"
#include "TPSEnemyAnimInstance.generated.h"

class UAnimMontage;

// 적 캐릭터 전용 FSM 상태를 애니메이션 블루프린트에 전달하는 애니메이션 인스턴스입니다.
UCLASS()
class TPSPROJECT_API UTPSEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 적 애니메이션 인스턴스의 기본 상태 값을 초기화하는 생성자입니다.
	UTPSEnemyAnimInstance();

	// 공격 애니메이션 종료 노티파이에서 호출되어 공격 재생 상태를 끄는 함수입니다.
	UFUNCTION()
	void AnimNotify_AttackEnd();

	// 피격 몽타주가 연결되어 있으면 재생하는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "Animation|Damage")
	void PlayDamageAnim();

	// 사망 몽타주가 연결되어 있으면 재생하는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "Animation|Dead")
	void PlayDeadAnim();

	// 애니메이션 블루프린트에서 확인할 현재 적 FSM 상태입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	ETPSEnemyState AnimState;

	// 공격 애니메이션을 재생 중인지 나타내는 값입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Attack")
	bool bAttackPlay;

	// 피격 시 재생할 데미지 애니메이션 몽타주입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Damage")
	TObjectPtr<UAnimMontage> DamageMontage;

	// 사망 시 재생할 사망 애니메이션 몽타주입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Dead")
	TObjectPtr<UAnimMontage> DeadMontage;
};
