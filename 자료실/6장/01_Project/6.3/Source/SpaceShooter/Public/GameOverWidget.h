// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPACESHOOTER_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Widget init and button binding
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonQuit;

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonRestart;

	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnRestartClicked();
};
