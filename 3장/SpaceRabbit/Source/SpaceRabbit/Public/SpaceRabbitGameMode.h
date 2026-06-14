#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceRabbitGameMode.generated.h"

class AActor;
class ACoin;
class UGameOverWidget;
class USpaceRabbitHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOverSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChangedSignature, int32, NewScore);

UCLASS()
class SPACERABBIT_API ASpaceRabbitGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpaceRabbitGameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetCurrentScore() const { return CurrentScore; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetGameOver();

	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsGameOver() const { return bIsGameOver; }

	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnGameOverSignature OnGameOver;

	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnScoreChangedSignature OnScoreChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<USpaceRabbitHUD> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UGameOverWidget> GameOverWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 CurrentScore = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	bool bIsGameOver = false;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "UI")
	TObjectPtr<USpaceRabbitHUD> HUDWidget;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UGameOverWidget> GameOverWidget;

private:
	void BindCoinCollected(ACoin* Coin);

	UFUNCTION()
	void HandleActorSpawned(AActor* SpawnedActor);

	UFUNCTION()
	void HandleCoinCollected(ACoin* Coin, AActor* Collector);
};
