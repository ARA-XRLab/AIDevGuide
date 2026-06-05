// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestroyZoneActor.generated.h"

UCLASS()
class SPACESHOOTER_API ADestroyZoneActor : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	ADestroyZoneActor();

	// Lifecycle
	virtual void Tick(float DeltaTime) override;

protected:
	// Lifecycle
	virtual void BeginPlay() override;

	// Internal
	UFUNCTION()
	void OnBoxCompOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Components
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComp;
};
