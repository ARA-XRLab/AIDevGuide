// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShootingGameMode.generated.h"

UCLASS()
class SPACESHOOTER_API AShootingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	AShootingGameMode();

	// Lifecycle
	virtual void BeginPlay() override;

	// API - Score
	int32 GetScore();
	void SetScore(int32 NewScore);
	void AddScore(int32 Amount);
	
	int32 GetHighScore();
	void SetHighScore(int32 NewHighScore);

	// API - UI
	void ShowGameOverUI();
	void UpdatePlayerHP(int32 CurHP);

	// API - Save/Load
	void SaveGame();
	void LoadGame(int32 DefaultHighScore = 0);

protected:
	// Config - Widget
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMainWidget> MainWidgetFactory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameOverWidget> GameOverWidgetFactory;

	// Runtime - Widget
	UPROPERTY()
	TObjectPtr<class UMainWidget> MainWidget;

	UPROPERTY()
	TObjectPtr<class UGameOverWidget> GameOverWidget;

private:
	// Internal State
	int32 HighScore = 0;
	int32 Score = 0;
};
