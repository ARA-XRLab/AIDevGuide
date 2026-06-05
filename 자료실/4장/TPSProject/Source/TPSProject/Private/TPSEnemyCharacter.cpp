// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSEnemyCharacter.h"

#include "TPSEnemyAnimInstance.h"
#include "TPSEnemyFSMComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

ATPSEnemyCharacter::ATPSEnemyCharacter()
{
	// 현재 단계에서는 AI 이동이나 매 프레임 처리가 없으므로 Tick을 비활성화합니다.
	PrimaryActorTick.bCanEverTick = false;

	// 컨트롤러나 마우스 시점 방향으로 몸이 직접 회전하지 않도록 설정합니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 이동 방향을 기준으로 적 캐릭터가 자동 회전하도록 설정합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 에디터에서 조정할 기본 회전 속도입니다.
	EnemyRotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// 에디터에서 조정할 기본 최대 이동 속도입니다.
	EnemyMaxWalkSpeed = 300.0f;

	GetCharacterMovement()->RotationRate = EnemyRotationRate;
	GetCharacterMovement()->MaxWalkSpeed = EnemyMaxWalkSpeed;
	GetCharacterMovement()->bRunPhysicsWithNoController = true;

	// 에디터에서 조정할 기본 스켈레탈 메시 위치 보정 값을 설정합니다.
	EnemyMeshRelativeLocation = FVector(0.0f, 0.0f, -90.0f);

	// 에디터에서 조정할 기본 스켈레탈 메시 회전 보정 값을 설정합니다.
	EnemyMeshRelativeRotation = FRotator(0.0f, -90.0f, 0.0f);

	// 적 몸통 캡슐은 이동 충돌만 담당하고 사격 판정에서는 제외합니다.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	// 코드에서 기본 스켈레탈 메시 슬롯에 적 메시를 직접 연결합니다.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> EnemyMeshAsset(TEXT("/Game/Enemy/Model/vampire_a_lusth.vampire_a_lusth"));
	if (EnemyMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(EnemyMeshAsset.Object);
	}

	// 에디터에서 만든 적 애니메이션 블루프린트 클래스를 기본 메시 슬롯에 연결합니다.
	static ConstructorHelpers::FClassFinder<UTPSEnemyAnimInstance> EnemyAnimBlueprintClass(TEXT("/Game/Animation/ABP_TPSEnemy"));
	if (EnemyAnimBlueprintClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(EnemyAnimBlueprintClass.Class);
	}

	// 적 메시는 물리 충돌 없이 사격 판정만 받도록 설정합니다.
	GetMesh()->SetRelativeLocation(EnemyMeshRelativeLocation);
	GetMesh()->SetRelativeRotation(EnemyMeshRelativeRotation);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(false);
	GetMesh()->SetSimulatePhysics(false);

	// 적 캐릭터의 상태 전환과 단순 추적 이동을 담당하는 FSM 컴포넌트를 생성합니다.
	EnemyFSMComponent = CreateDefaultSubobject<UTPSEnemyFSMComponent>(TEXT("EnemyFSMComponent"));

	// 에디터에서 조정할 기본 HP 값을 설정합니다.
	hp = 3;
	maxHp = 3;

	// 사망 처리는 한 번만 실행되도록 초기 상태를 저장합니다.
	bIsDead = false;
}

void ATPSEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 스폰된 적은 기본 설정상 컨트롤러가 없을 수 있으므로 이동 물리를 직접 허용합니다.
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->bRunPhysicsWithNoController = true;

		if (MovementComponent->MovementMode == MOVE_None)
		{
			MovementComponent->SetDefaultMovementMode();
		}
	}
}

void ATPSEnemyCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 에디터에서 조정한 스켈레탈 메시 보정 값을 기본 메시 슬롯에 반영합니다.
	GetMesh()->SetRelativeLocation(EnemyMeshRelativeLocation);
	GetMesh()->SetRelativeRotation(EnemyMeshRelativeRotation);

	// 에디터에서 조정한 이동 컴포넌트 값을 반영합니다.
	GetCharacterMovement()->RotationRate = EnemyRotationRate;
	GetCharacterMovement()->MaxWalkSpeed = EnemyMaxWalkSpeed;
}

float ATPSEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return 0.0f;
	}

	// 기본 데미지 이벤트 처리를 유지하고 받은 데미지 수치를 경고 로그로 출력합니다.
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("Enemy Received Damage: %.2f"), DamageAmount);

	if (DamageAmount > 0.0f)
	{
		hp = FMath::Max(0, hp - 1);

		if (hp <= 0)
		{
			HandleDead();
		}
		else
		{
			if (EnemyFSMComponent)
			{
				EnemyFSMComponent->NotifyHit();
			}

			if (UTPSEnemyAnimInstance* EnemyAnimInstance = Cast<UTPSEnemyAnimInstance>(GetMesh()->GetAnimInstance()))
			{
				EnemyAnimInstance->PlayDamageAnim();
			}
		}
	}

	return AppliedDamage;
}

void ATPSEnemyCharacter::HandleDead()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	if (EnemyFSMComponent)
	{
		EnemyFSMComponent->NotifyDead();
	}
}
