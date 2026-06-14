// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSPlayerAnimInstance.generated.h"

class UAnimMontage;

// 플레이어 캐릭터 전용 이동 애니메이션 값을 계산하는 애니메이션 인스턴스입니다.
UCLASS()
class TPSPROJECT_API UTPSPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 플레이어 애니메이션 인스턴스의 기본 값을 초기화하는 생성자입니다.
	UTPSPlayerAnimInstance();

	// 애니메이션 갱신 시 플레이어 이동 값을 매 프레임 계산하는 함수입니다.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 공격 몽타주가 연결되어 있으면 중복 없이 재생하는 함수입니다.
	UFUNCTION(BlueprintCallable, Category = "Animation|Attack")
	void PlayAttackAnim();

	// 캐릭터 앞 방향 기준 앞뒤 이동 속도입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Speed;

	// 캐릭터 오른쪽 방향 기준 좌우 이동 속도입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float Direction;

	// 캐릭터가 공중에 떠 있는지 나타내는 값입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsInAir;

	// 사격 시 재생할 공격 애니메이션 몽타주입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Attack")
	TObjectPtr<UAnimMontage> AttackMontage;
};
