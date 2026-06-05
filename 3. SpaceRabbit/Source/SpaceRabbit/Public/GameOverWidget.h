#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class SPACERABBIT_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Game Over")
	void SetFinalScore(int32 Score);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Game Over")
	TObjectPtr<UTextBlock> FinalScoreText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Game Over")
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Game Over")
	TObjectPtr<UButton> QuitButton;

private:
	UFUNCTION()
	void HandleRestartButtonClicked();

	UFUNCTION()
	void HandleQuitButtonClicked();
};
