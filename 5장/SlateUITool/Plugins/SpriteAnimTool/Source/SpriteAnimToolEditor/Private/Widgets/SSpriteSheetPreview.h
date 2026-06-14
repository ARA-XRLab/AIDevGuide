#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SLeafWidget.h"

class UTexture2D;

class SSpriteSheetPreview : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SSpriteSheetPreview) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetTexture(UTexture2D* InTexture);
	void SetGridOverlay(int32 InRows, int32 InColumns, int32 InValidFrameCount);
	void ClearGridOverlay();
	void SetFramePreview(int32 InRows, int32 InColumns, int32 InFrameIndex);
	void ClearFramePreview();

	virtual int32 OnPaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

private:
	FSlateRect GetDrawRect(const FVector2D& ViewSize, const FVector2D& SourceSize) const;

	TObjectPtr<UTexture2D> Texture = nullptr;
	FSlateBrush PreviewBrush;
	int32 GridRows = 1;
	int32 GridColumns = 1;
	int32 ValidFrameCount = 0;
	int32 PreviewFrameIndex = 0;
	bool bShowGridOverlay = false;
	bool bShowSingleFrame = false;
};
