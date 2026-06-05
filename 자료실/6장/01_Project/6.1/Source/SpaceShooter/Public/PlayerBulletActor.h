// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerBulletActor.generated.h"

UCLASS()
class SPACESHOOTER_API ABulletActor : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	ABulletActor();

	// Lifecycle
	virtual void Tick(float DeltaTime) override;

	// API
	void SetActive(bool bActive);

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

	// Config - Mesh
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UStaticMesh> BulletMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMaterialInterface> BulletMaterial;

	// Config - Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 1000.f;

	// Config - VFX/Sound
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UParticleSystem> ExplosionVFX;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundCue> ExplosionSound;
};
