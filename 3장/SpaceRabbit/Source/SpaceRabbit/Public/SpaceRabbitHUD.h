#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceRabbitHUD.generated.h"

class ASpaceRabbitGameMode;
class UTextBlock;

UCLASS()
class SPACERABBIT_API USpaceRabbitHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateScore(int32 NewScore);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "HUD")
	TObjectPtr<UTextBlock> ScoreText;

private:
	UFUNCTION()
	void HandleScoreChanged(int32 NewScore);

	TWeakObjectPtr<ASpaceRabbitGameMode> CachedGameMode;
};
