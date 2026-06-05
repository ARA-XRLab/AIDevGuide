// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBulletActor.generated.h"

UCLASS()
class SPACESHOOTER_API AEnemyBulletActor : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	AEnemyBulletActor();

	// Lifecycle
	virtual void Tick(float DeltaTime) override;

	// API
	void SetActive(bool bActive);
	void ResetBullet();
	bool IsActive() const;

protected:
	// Lifecycle
	virtual void BeginPlay() override;

	// Internal
	UFUNCTION()
	void OnBoxCompOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnLifeTimeExpired();

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* MeshComp;

	// Config - Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 800.f;

	// Config - Lifetime
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTime = 5.f;

	// Config - Damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage = 1;

private:
	// Internal State
	bool bIsActive = false;
	FTimerHandle LifeTimeTimerHandle;
};
