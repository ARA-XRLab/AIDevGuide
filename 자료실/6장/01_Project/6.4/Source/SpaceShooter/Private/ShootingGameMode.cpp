// Fill out your copyright notice in the Description page of Project Settings.

#include "ShootingGameMode.h"

#include "GameOverWidget.h"
#include "MainWidget.h"

AShootingGameMode::AShootingGameMode()
{
}

void AShootingGameMode::BeginPlay()
{
	Super::BeginPlay();

	MainWidget = CreateWidget<UMainWidget>(GetWorld(), MainWidgetFactory);
	MainWidget->AddToViewport();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->SetShowMouseCursor(false);

	LoadGame();
	MainWidget->UpdateTextScore(HighScore, Score);
}

int32 AShootingGameMode::GetScore()
{
	return Score;
}

void AShootingGameMode::SetScore(int32 NewScore)
{
	Score = NewScore;

	if (Score > HighScore)
	{
		HighScore = Score;
		SaveGame();
	}

	MainWidget->UpdateTextScore(HighScore, Score);
}

int32 AShootingGameMode::GetHighScore()
{
	return HighScore;
}

void AShootingGameMode::SetHighScore(int32 NewHighScore)
{
	HighScore = NewHighScore;
}

void AShootingGameMode::AddScore(int32 Amount)
{
	SetScore(Score + Amount);
}

void AShootingGameMode::UpdatePlayerHP(int32 CurHP)
{
	MainWidget->UpdatePlayerHP(CurHP);
}

void AShootingGameMode::ShowGameOverUI()
{
	GameOverWidget = CreateWidget<UGameOverWidget>(GetWorld(), GameOverWidgetFactory);
	GameOverWidget->AddToViewport(10);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->SetShowMouseCursor(true);
}

void AShootingGameMode::SaveGame()
{
	GConfig->SetInt(TEXT("GameData"), TEXT("HighScore"), HighScore, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void AShootingGameMode::LoadGame(int32 DefaultHighScore)
{
	if (!GConfig->GetInt(TEXT("GameData"), TEXT("HighScore"), HighScore, GGameUserSettingsIni))
	{
		HighScore = DefaultHighScore;
	}
}
