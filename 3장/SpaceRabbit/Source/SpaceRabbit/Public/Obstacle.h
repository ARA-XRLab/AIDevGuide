#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;

UCLASS()
class SPACERABBIT_API AObstacle : public AActor
{
	GENERATED_BODY()

public:
	AObstacle();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPaperSpriteComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	int32 Damage = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	float MoveSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obstacle")
	float DestroyY = -1000.0f;

private:
	UFUNCTION()
	void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void MoveObstacle(float DeltaSeconds);
	void UpdateDestroyState();
};
