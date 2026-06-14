#include "Widgets/SSpriteSheetPreview.h"

#include "Engine/Texture2D.h"
#include "Rendering/DrawElements.h"
#include "Styling/AppStyle.h"

void SSpriteSheetPreview::Construct(const FArguments& InArgs)
{
	PreviewBrush.DrawAs = ESlateBrushDrawType::Image;
}

void SSpriteSheetPreview::SetTexture(UTexture2D* InTexture)
{
	Texture = InTexture;
	PreviewBrush.SetResourceObject(Texture);

	if (Texture != nullptr)
	{
		PreviewBrush.ImageSize = FVector2D(Texture->GetSurfaceWidth(), Texture->GetSurfaceHeight());
	}
	else
	{
		PreviewBrush.ImageSize = FVector2D::ZeroVector;
	}

	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

void SSpriteSheetPreview::SetGridOverlay(int32 InRows, int32 InColumns, int32 InValidFrameCount)
{
	GridRows = FMath::Max(1, InRows);
	GridColumns = FMath::Max(1, InColumns);
	ValidFrameCount = FMath::Clamp(InValidFrameCount, 0, GridRows * GridColumns);
	bShowGridOverlay = true;
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

void SSpriteSheetPreview::ClearGridOverlay()
{
	bShowGridOverlay = false;
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

void SSpriteSheetPreview::SetFramePreview(int32 InRows, int32 InColumns, int32 InFrameIndex)
{
	GridRows = FMath::Max(1, InRows);
	GridColumns = FMath::Max(1, InColumns);
	PreviewFrameIndex = FMath::Max(0, InFrameIndex);
	bShowSingleFrame = true;
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

void SSpriteSheetPreview::ClearFramePreview()
{
	bShowSingleFrame = false;
	Invalidate(EInvalidateWidgetReason::PaintAndVolatility);
}

int32 SSpriteSheetPreview::OnPaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	if (Texture == nullptr)
	{
		return LayerId;
	}

	const FVector2D PreviewSize = AllottedGeometry.GetLocalSize();
	const float TextureWidth = static_cast<float>(Texture->GetSurfaceWidth());
	const float TextureHeight = static_cast<float>(Texture->GetSurfaceHeight());

	if (PreviewSize.X <= 0.0f || PreviewSize.Y <= 0.0f || TextureWidth <= 0.0f || TextureHeight <= 0.0f)
	{
		return LayerId;
	}

	const FVector2D SourceSize = bShowSingleFrame
		? FVector2D(TextureWidth / static_cast<float>(GridColumns), TextureHeight / static_cast<float>(GridRows))
		: FVector2D(TextureWidth, TextureHeight);
	const FSlateRect DrawRect = GetDrawRect(PreviewSize, SourceSize);
	const FVector2D DrawSize(DrawRect.Right - DrawRect.Left, DrawRect.Bottom - DrawRect.Top);
	FSlateBrush DrawBrush = PreviewBrush;

	if (bShowSingleFrame)
	{
		const int32 FrameCount = FMath::Max(1, GridRows * GridColumns);
		const int32 ClampedFrameIndex = FMath::Clamp(PreviewFrameIndex, 0, FrameCount - 1);
		const int32 FrameRow = ClampedFrameIndex / GridColumns;
		const int32 FrameColumn = ClampedFrameIndex % GridColumns;
		const FVector2f UVMin(
			static_cast<float>(FrameColumn) / static_cast<float>(GridColumns),
			static_cast<float>(FrameRow) / static_cast<float>(GridRows));
		const FVector2f UVMax(
			static_cast<float>(FrameColumn + 1) / static_cast<float>(GridColumns),
			static_cast<float>(FrameRow + 1) / static_cast<float>(GridRows));
		DrawBrush.SetUVRegion(FBox2f(UVMin, UVMax));
	}
	else
	{
		DrawBrush.SetUVRegion(FBox2f(FVector2f(0.0f, 0.0f), FVector2f(1.0f, 1.0f)));
	}

	const FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry(
		DrawSize,
		FSlateLayoutTransform(FVector2D(DrawRect.Left, DrawRect.Top)));

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		&DrawBrush,
		ESlateDrawEffect::None,
		InWidgetStyle.GetColorAndOpacityTint());

	if (bShowGridOverlay && !bShowSingleFrame)
	{
		const float CellWidth = DrawSize.X / static_cast<float>(GridColumns);
		const float CellHeight = DrawSize.Y / static_cast<float>(GridRows);
		const FLinearColor ValidLineColor(0.1f, 0.9f, 0.1f, 1.0f);
		const FLinearColor InvalidFillColor(0.75f, 0.1f, 0.1f, 0.24f);
		const FLinearColor InvalidLineColor(0.92f, 0.16f, 0.16f, 1.0f);
		const FSlateBrush* WhiteBrush = FAppStyle::GetBrush("WhiteBrush");

		for (int32 CellIndex = ValidFrameCount; CellIndex < GridRows * GridColumns; ++CellIndex)
		{
			const int32 CellRow = CellIndex / GridColumns;
			const int32 CellColumn = CellIndex % GridColumns;
			const FVector2D CellPosition(
				DrawRect.Left + (CellWidth * static_cast<float>(CellColumn)),
				DrawRect.Top + (CellHeight * static_cast<float>(CellRow)));
			const FPaintGeometry CellGeometry = AllottedGeometry.ToPaintGeometry(
				FVector2D(CellWidth, CellHeight),
				FSlateLayoutTransform(CellPosition));

			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId + 1,
				CellGeometry,
				WhiteBrush,
				ESlateDrawEffect::None,
				InvalidFillColor);
		}

		for (int32 RowIndex = 0; RowIndex <= GridRows; ++RowIndex)
		{
			const float LineY = DrawRect.Top + (CellHeight * static_cast<float>(RowIndex));
			TArray<FVector2f> LinePoints;
			LinePoints.Add(FVector2f(DrawRect.Left, LineY));
			LinePoints.Add(FVector2f(DrawRect.Right, LineY));

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId + 2,
				AllottedGeometry.ToPaintGeometry(),
				LinePoints,
				ESlateDrawEffect::None,
				ValidLineColor,
				true,
				1.5f);
		}

		for (int32 ColumnIndex = 0; ColumnIndex <= GridColumns; ++ColumnIndex)
		{
			const float LineX = DrawRect.Left + (CellWidth * static_cast<float>(ColumnIndex));
			TArray<FVector2f> LinePoints;
			LinePoints.Add(FVector2f(LineX, DrawRect.Top));
			LinePoints.Add(FVector2f(LineX, DrawRect.Bottom));

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId + 2,
				AllottedGeometry.ToPaintGeometry(),
				LinePoints,
				ESlateDrawEffect::None,
				ValidLineColor,
				true,
				1.5f);
		}

		for (int32 CellIndex = ValidFrameCount; CellIndex < GridRows * GridColumns; ++CellIndex)
		{
			const int32 CellRow = CellIndex / GridColumns;
			const int32 CellColumn = CellIndex % GridColumns;
			const float Left = DrawRect.Left + (CellWidth * static_cast<float>(CellColumn));
			const float Top = DrawRect.Top + (CellHeight * static_cast<float>(CellRow));
			const float Right = Left + CellWidth;
			const float Bottom = Top + CellHeight;
			TArray<FVector2f> CellOutline;
			CellOutline.Add(FVector2f(Left, Top));
			CellOutline.Add(FVector2f(Right, Top));
			CellOutline.Add(FVector2f(Right, Bottom));
			CellOutline.Add(FVector2f(Left, Bottom));
			CellOutline.Add(FVector2f(Left, Top));

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId + 3,
				AllottedGeometry.ToPaintGeometry(),
				CellOutline,
				ESlateDrawEffect::None,
				InvalidLineColor,
				true,
				2.0f);
		}

		return LayerId + 3;
	}

	return LayerId;
}

FVector2D SSpriteSheetPreview::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(256.0f, 256.0f);
}

FSlateRect SSpriteSheetPreview::GetDrawRect(const FVector2D& ViewSize, const FVector2D& SourceSize) const
{
	if (ViewSize.X <= 0.0f || ViewSize.Y <= 0.0f || SourceSize.X <= 0.0f || SourceSize.Y <= 0.0f)
	{
		return FSlateRect(0.0f, 0.0f, 0.0f, 0.0f);
	}

	const float Scale = FMath::Min(ViewSize.X / SourceSize.X, ViewSize.Y / SourceSize.Y);
	const FVector2D DrawSize(SourceSize.X * Scale, SourceSize.Y * Scale);
	const FVector2D DrawOffset((ViewSize.X - DrawSize.X) * 0.5f, (ViewSize.Y - DrawSize.Y) * 0.5f);

	return FSlateRect(
		DrawOffset.X,
		DrawOffset.Y,
		DrawOffset.X + DrawSize.X,
		DrawOffset.Y + DrawSize.Y);
}
