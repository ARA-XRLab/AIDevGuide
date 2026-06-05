// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyActor.generated.h"

UCLASS()
class SPACESHOOTER_API AEnemyActor : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyActor();

	// Lifecycle
	virtual void Tick(float DeltaTime) override;

protected:
	// Internal - Combat
	void TryStartFiring();
	void FireOneBullet();
	void EndFiring();

	// API
	int32 GetHP();
	void SetHP(int32 NewHP);
	void AddHP(int32 Amount);

protected:
	// Lifecycle
	virtual void BeginPlay() override;

	// Internal
	UFUNCTION()
	void OnBoxCompOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* MeshComp;

	// Config - Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 500.f;

	// Config - VFX/Sound
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UParticleSystem> ExplosionVFX;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> FireSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> ExplosionSound;

	// Config - HP
	int32 MaxHP = 2;

	// Runtime
	FVector MoveDirection;
	int32 CurHP;


	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<class AEnemyBulletActor> EnemyBulletClass;

	UPROPERTY(EditAnywhere, Category="Combat", meta=(ClampMin="0.0"))
	float EncounterRadius = 500.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float FireInterval = 0.2f;

	UPROPERTY(EditAnywhere, Category="Combat")
	int32 MinBurstCount = 3;

	UPROPERTY(EditAnywhere, Category="Combat")
	int32 MaxBurstCount = 5;

	// Runtime - Combat
	bool bHasFired = false;
	bool bIsFiring = false;
	int32 CurrentBurstCount = 0;
	int32 TargetBurstCount = 0;
	FVector FireDirection;
	FTimerHandle FireTimerHandle;
};
