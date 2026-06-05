#include "SpaceRabbitHUD.h"

#include "Components/TextBlock.h"
#include "SpaceRabbitGameMode.h"

void USpaceRabbitHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		if (ASpaceRabbitGameMode* SpaceRabbitGameMode = World->GetAuthGameMode<ASpaceRabbitGameMode>())
		{
			CachedGameMode = SpaceRabbitGameMode;

			if (!SpaceRabbitGameMode->OnScoreChanged.IsAlreadyBound(this, &USpaceRabbitHUD::HandleScoreChanged))
			{
				SpaceRabbitGameMode->OnScoreChanged.AddDynamic(this, &USpaceRabbitHUD::HandleScoreChanged);
			}

			UpdateScore(SpaceRabbitGameMode->GetCurrentScore());
		}
	}
}

void USpaceRabbitHUD::NativeDestruct()
{
	if (ASpaceRabbitGameMode* SpaceRabbitGameMode = CachedGameMode.Get())
	{
		SpaceRabbitGameMode->OnScoreChanged.RemoveDynamic(this, &USpaceRabbitHUD::HandleScoreChanged);
	}

	Super::NativeDestruct();
}

void USpaceRabbitHUD::UpdateScore(int32 NewScore)
{
	if (ScoreText == nullptr)
	{
		return;
	}

	ScoreText->SetText(FText::AsNumber(NewScore));
}

void USpaceRabbitHUD::HandleScoreChanged(int32 NewScore)
{
	UpdateScore(NewScore);
}
