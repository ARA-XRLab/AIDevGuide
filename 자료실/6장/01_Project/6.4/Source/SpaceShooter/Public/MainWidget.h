// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"


UCLASS()
class SPACESHOOTER_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextHighScore;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextScore;

	UPROPERTY(meta=(BindWidget))
	class UImage* ImagePlayer00;
	UPROPERTY(meta=(BindWidget))
	class UImage* ImagePlayer01;
	UPROPERTY(meta=(BindWidget))
	class UImage* ImagePlayer02;
	
	void UpdateTextScore(int32 HighScore, int32 Score);

	void UpdatePlayerHP(int32 CurHP);
};
