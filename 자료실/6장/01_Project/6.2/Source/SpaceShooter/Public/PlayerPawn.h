// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class SPACESHOOTER_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Constructor
	APlayerPawn();

	// Lifecycle
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// API
	int32 GetHP();
	void SetHP(int32 NewHP);
	void OnDamage(int32 Amount);
	bool IsInvincible() const;
	void FireBullet();
	void ReturnBulletToPool(class ABulletActor* Bullet);

protected:
	// Lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Internal
	void OnInputMove(const FInputActionValue& Value);
	void OnInputFirePressed(const FInputActionValue& Value);
	void OnInputFireReleased(const FInputActionValue& Value);
	
	void Die();
	void StartInvincibility();
	void EndInvincibility();
	void BlinkMesh();

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UArrowComponent* FirePointComp;

	// Config - Input
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_Player;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Move;

	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Fire;

	// Config - Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TiltAngle = -35.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TiltInterpSpeed = 5.f;

	// Config - Bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABulletActor> BulletFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxBulletCount = 10;

	// Config - HP
	int32 MaxHP = 3;

	// Config - Hit Effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InvincibilityDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlinkInterval = 0.1f;

	// Config - VFX/Sound
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UParticleSystem> ExplosionVFX;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> FireSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> ExplosionSound;

	// Runtime
	int32 CurHP = 0;
	bool bIsInvincible = false;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

private:
	// Internal State
	UPROPERTY(VisibleAnywhere)
	TArray<class ABulletActor*> BulletPool;

	float InputH = 0.f;
	float InputV = 0.f;
	float FireInterval = 0.1f;
	FTimerHandle FireTimerHandle;
	FTimerHandle InvincibilityTimerHandle;
	FTimerHandle BlinkTimerHandle;
	bool bBlinkVisible = true;
};
