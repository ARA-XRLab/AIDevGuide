// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonQuit->OnClicked.AddDynamic(this, &UGameOverWidget::OnQuitClicked);
	ButtonRestart->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartClicked);
}

void UGameOverWidget::OnQuitClicked()
{
	// 종료 처리를 하고싶다.
	auto* pc = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(GetWorld(), pc, EQuitPreference::Quit, false);
}

void UGameOverWidget::OnRestartClicked()
{
	// 재시작 하고싶다.
	FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(*levelName));
}
