// Fill out your copyright notice in the Description page of Project Settings.

#include "ShootingGameMode.h"

#include "EngineUtils.h"
#include "GameOverWidget.h"
#include "MainWidget.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

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

	GetWorldTimerManager().SetTimer(
		ActorTrackingTimerHandle,
		this,
		&AShootingGameMode::LogActorTracking,
		5.0f,
		true,
		5.0f
	);
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

void AShootingGameMode::LogActorTracking()
{
	if (!GetWorld()) return;

	TMap<FString, int32> ClassCounts;
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor) continue;
        
		const FString ClassName = Actor->GetClass()->GetName();
		ClassCounts.FindOrAdd(ClassName)++;
	}

	if (ClassCounts.Num() == 0) return;

	const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y.%m.%d-%H:%M:%S"));

	TArray<FString> ClassNames;
	ClassCounts.GetKeys(ClassNames);
	ClassNames.Sort();
    
	FString LogLines;
	for (const FString& ClassName : ClassNames)
	{
		const int32 Count = ClassCounts[ClassName];
		LogLines += FString::Printf(TEXT("[%s] %s: %d\n"), 
			*Timestamp, *ClassName, Count);
	}

	const FString LogFilePath = FPaths::Combine( FPaths::ProjectLogDir(), TEXT("ActorTracking.log"));
	FFileHelper::SaveStringToFile(
		LogLines, 
		*LogFilePath, 
		FFileHelper::EEncodingOptions::AutoDetect, 
		&IFileManager::Get(), 
		FILEWRITE_Append
	);
}