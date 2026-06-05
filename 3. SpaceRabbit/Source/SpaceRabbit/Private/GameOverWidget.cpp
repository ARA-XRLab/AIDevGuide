#include "GameOverWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton != nullptr && !RestartButton->OnClicked.IsAlreadyBound(this, &UGameOverWidget::HandleRestartButtonClicked))
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::HandleRestartButtonClicked);
	}

	if (QuitButton != nullptr && !QuitButton->OnClicked.IsAlreadyBound(this, &UGameOverWidget::HandleQuitButtonClicked))
	{
		QuitButton->OnClicked.AddDynamic(this, &UGameOverWidget::HandleQuitButtonClicked);
	}
}

void UGameOverWidget::NativeDestruct()
{
	if (RestartButton != nullptr)
	{
		RestartButton->OnClicked.RemoveDynamic(this, &UGameOverWidget::HandleRestartButtonClicked);
	}

	if (QuitButton != nullptr)
	{
		QuitButton->OnClicked.RemoveDynamic(this, &UGameOverWidget::HandleQuitButtonClicked);
	}

	Super::NativeDestruct();
}

void UGameOverWidget::SetFinalScore(int32 Score)
{
	if (FinalScoreText == nullptr)
	{
		return;
	}

	FinalScoreText->SetText(FText::AsNumber(Score));
}

void UGameOverWidget::HandleRestartButtonClicked()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(this, FName(*World->GetName()));
	}
}

void UGameOverWidget::HandleQuitButtonClicked()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
	}
}
