// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMainWidget::UpdateTextScore(int32 HighScore, int32 Score)
{
	TextHighScore->SetText(FText::AsNumber(HighScore));
	TextScore->SetText(FText::AsNumber(Score));
}

void UMainWidget::UpdatePlayerHP(int32 CurHP)
{
	ImagePlayer00->SetVisibility(ESlateVisibility::Hidden);
	ImagePlayer01->SetVisibility(ESlateVisibility::Hidden);
	ImagePlayer02->SetVisibility(ESlateVisibility::Hidden);

	if (CurHP >= 1)
		ImagePlayer00->SetVisibility(ESlateVisibility::Visible);

	if (CurHP >= 2)
		ImagePlayer01->SetVisibility(ESlateVisibility::Visible);

	if (CurHP >= 3)
		ImagePlayer02->SetVisibility(ESlateVisibility::Visible);
}
