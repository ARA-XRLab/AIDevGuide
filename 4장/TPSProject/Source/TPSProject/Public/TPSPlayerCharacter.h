// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TPSPlayerCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UParticleSystem;
class USkeletalMeshComponent;
class USoundBase;
class USpringArmComponent;
class UUserWidget;

// TPS 프로젝트에서 플레이어가 조종할 기본 캐릭터 클래스입니다.
UCLASS()
class TPSPROJECT_API ATPSPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 플레이어 캐릭터의 기본 설정을 초기화하는 생성자입니다.
	ATPSPlayerCharacter();

	// 에디터에서 변경된 카메라 거리를 스프링 암에 반영하는 함수입니다.
	virtual void OnConstruction(const FTransform& Transform) override;

	// 플레이어 입력 컴포넌트에 Enhanced Input 액션을 바인딩하는 함수입니다.
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	// 게임 시작 시 플레이어 입력 매핑 컨텍스트를 등록하는 함수입니다.
	virtual void BeginPlay() override;

	// 마우스 시점 방향을 따라 회전하며 카메라 거리를 담당하는 스프링 암입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	// 스프링 암 끝에 붙는 3인칭 시점 카메라입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	// 에디터에서 직접 조정할 수 있는 카메라 거리입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float CameraDistance;

	// 기본 스켈레탈 메시의 위치 보정 값입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FVector PlayerMeshRelativeLocation;

	// 기본 스켈레탈 메시의 회전 보정 값입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FRotator PlayerMeshRelativeRotation;

	// 플레이어 오른손 소켓에 부착할 총기 스켈레탈 메시 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> RifleMeshComp;

	// 게임 시작 시 등록할 기본 Enhanced Input 매핑 컨텍스트입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// 카메라 Yaw 기준 앞뒤와 좌우 이동을 처리하는 입력 액션입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	// 마우스 좌우와 상하 시점 회전을 처리하는 입력 액션입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// Spacebar를 누르는 순간 점프를 실행하는 입력 액션입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	// 마우스 클릭 순간 사격을 실행하는 입력 액션입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	// 카메라가 바라보는 방향으로 발사 판정을 수행할 최대 거리입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float FireTraceDistance;

	// 적에게 사격이 맞았을 때 전달할 데미지 수치입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float FireDamage;

	// 총알이 맞은 지점에 1회 생성할 이펙트입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UParticleSystem> BulletImpactFX;

	// 이펙트가 표면에 파묻히지 않도록 법선 방향으로 띄우는 거리입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float BulletImpactFXOffset;

	// 사격 시 2D로 재생할 총소리 사운드입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USoundBase> BulletSound;

	// 에디터에서 연결할 크로스헤어 위젯 클래스입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	// 게임 시작 시 생성되어 화면에 표시되는 크로스헤어 위젯입니다.
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> CrosshairWidget;

	// 입력 값 X/Y를 카메라 Yaw 기준 이동 방향으로 변환해 캐릭터를 이동시키는 함수입니다.
	void Move(const FInputActionValue& Value);

	// 입력 값 X/Y를 컨트롤러 Yaw/Pitch 회전에 반영해 시점을 회전시키는 함수입니다.
	void Look(const FInputActionValue& Value);

	// 카메라 위치와 방향을 기준으로 직선 사격 판정을 수행하는 함수입니다.
	void Fire();

	// 맞은 지점과 표면 법선을 기준으로 총알 충돌 이펙트를 생성하는 함수입니다.
	void SpawnBulletImpactFX(const FHitResult& HitResult);

	// 맞은 대상이 적 캐릭터일 때 기본 데미지 타입으로 데미지를 전달하는 함수입니다.
	void ApplyDamageToHitEnemy(AActor* HitActor);
};
