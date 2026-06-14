// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPlayerAnimInstance.h"

#include "TPSPlayerCharacter.h"

#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"

UTPSPlayerAnimInstance::UTPSPlayerAnimInstance()
{
	// 애니메이션 블루프린트에서 확인할 기본 이동 값을 0으로 초기화합니다.
	Speed = 0.0f;

	// 애니메이션 블루프린트에서 확인할 기본 좌우 이동 값을 0으로 초기화합니다.
	Direction = 0.0f;

	// 시작 시에는 지상 상태로 초기화합니다.
	bIsInAir = false;

	// 공격 몽타주는 에디터에서 연결합니다.
	AttackMontage = nullptr;
}

void UTPSPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 이 애니메이션을 소유한 폰을 플레이어 캐릭터로 변환합니다.
	ATPSPlayerCharacter* PlayerCharacter = Cast<ATPSPlayerCharacter>(TryGetPawnOwner());
	if (!PlayerCharacter)
	{
		Speed = 0.0f;
		Direction = 0.0f;
		bIsInAir = false;
		return;
	}

	// 수평 이동 속도를 캐릭터 기준 앞 방향과 오른쪽 방향으로 분해합니다.
	const FVector Velocity = PlayerCharacter->GetVelocity();
	const FVector HorizontalVelocity(Velocity.X, Velocity.Y, 0.0f);
	Speed = FVector::DotProduct(HorizontalVelocity, PlayerCharacter->GetActorForwardVector());
	Direction = FVector::DotProduct(HorizontalVelocity, PlayerCharacter->GetActorRightVector());

	// 캐릭터 이동 컴포넌트가 낙하 상태인지 확인해 공중 여부를 갱신합니다.
	const UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
	bIsInAir = MovementComponent ? MovementComponent->IsFalling() : false;
}

void UTPSPlayerAnimInstance::PlayAttackAnim()
{
	if (!AttackMontage)
	{
		return;
	}

	if (Montage_IsPlaying(AttackMontage))
	{
		return;
	}

	Montage_Play(AttackMontage);
}
