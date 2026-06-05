#include "SpaceRabbitGameMode.h"

#include "Coin.h"
#include "Blueprint/UserWidget.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameOverWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpaceRabbitCharacter.h"
#include "SpaceRabbitHUD.h"

ASpaceRabbitGameMode::ASpaceRabbitGameMode()
{
	DefaultPawnClass = ASpaceRabbitCharacter::StaticClass();
	HUDWidgetClass = USpaceRabbitHUD::StaticClass();
	GameOverWidgetClass = UGameOverWidget::StaticClass();
}

void ASpaceRabbitGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentScore = 0;
	bIsGameOver = false;
	GameOverWidget = nullptr;

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);

		if (HUDWidgetClass != nullptr)
		{
			HUDWidget = CreateWidget<USpaceRabbitHUD>(PlayerController, HUDWidgetClass);
			if (HUDWidget != nullptr)
			{
				HUDWidget->AddToViewport();
			}
		}
	}

	OnScoreChanged.Broadcast(CurrentScore);

	if (UWorld* World = GetWorld())
	{
		World->AddOnActorSpawnedHandler(
			FOnActorSpawned::FDelegate::CreateUObject(this, &ASpaceRabbitGameMode::HandleActorSpawned));

		for (TActorIterator<ACoin> CoinIterator(World); CoinIterator; ++CoinIterator)
		{
			BindCoinCollected(*CoinIterator);
		}
	}
}

void ASpaceRabbitGameMode::AddScore(int32 Amount)
{
	if (bIsGameOver || Amount <= 0)
	{
		return;
	}

	CurrentScore += Amount;
	OnScoreChanged.Broadcast(CurrentScore);
	
	UE_LOG(LogTemp, Warning, TEXT("Score : %d"), CurrentScore);
}

void ASpaceRabbitGameMode::SetGameOver()
{
	if (bIsGameOver)
	{
		return;
	}

	bIsGameOver = true;
	OnGameOver.Broadcast();

	if (HUDWidget != nullptr)
	{
		HUDWidget->RemoveFromParent();
	}

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (GameOverWidgetClass != nullptr)
		{
			GameOverWidget = CreateWidget<UGameOverWidget>(PlayerController, GameOverWidgetClass);
			if (GameOverWidget != nullptr)
			{
				GameOverWidget->AddToViewport(10);
				GameOverWidget->SetFinalScore(CurrentScore);
			}
		}

		FInputModeUIOnly InputMode;
		if (GameOverWidget != nullptr)
		{
			InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
		}

		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(true);
	}
}

void ASpaceRabbitGameMode::BindCoinCollected(ACoin* Coin)
{
	if (Coin == nullptr || Coin->OnCoinCollected.IsAlreadyBound(this, &ASpaceRabbitGameMode::HandleCoinCollected))
	{
		return;
	}

	Coin->OnCoinCollected.AddDynamic(this, &ASpaceRabbitGameMode::HandleCoinCollected);
}

void ASpaceRabbitGameMode::HandleActorSpawned(AActor* SpawnedActor)
{
	BindCoinCollected(Cast<ACoin>(SpawnedActor));
}

void ASpaceRabbitGameMode::HandleCoinCollected(ACoin* Coin, AActor* Collector)
{
	if (Coin == nullptr)
	{
		return;
	}

	AddScore(Coin->GetCoinValue());
}
