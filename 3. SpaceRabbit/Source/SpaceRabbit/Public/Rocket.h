#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rocket.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;

UCLASS()
class SPACERABBIT_API ARocket : public AActor
{
	GENERATED_BODY()

public:
	ARocket();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPaperSpriteComponent> MeshComponent;

	// 로켓 직선 이동 속도 (units/초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rocket")
	float RocketSpeed = 500.0f;

	// 화면 왼쪽 밖으로 나갔다고 판단할 Y 좌표
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rocket")
	float DestroyY = -1500.0f;

private:
	UFUNCTION()
	void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void MoveRocket(float DeltaSeconds);
	void UpdateDestroyState();
};
