#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Coin.generated.h"

class AActor;
class USphereComponent;
class UPaperSpriteComponent;
class ACoin;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCoinCollectedSignature, ACoin*, Coin, AActor*, Collector);

UCLASS()
class SPACERABBIT_API ACoin : public AActor
{
	GENERATED_BODY()

public:
	ACoin();

	UFUNCTION(BlueprintPure, Category = "Coin")
	int32 GetCoinValue() const { return CoinValue; }

	UPROPERTY(BlueprintAssignable, Category = "Coin")
	FCoinCollectedSignature OnCoinCollected;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPaperSpriteComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coin")
	int32 CoinValue = 1;

private:
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
