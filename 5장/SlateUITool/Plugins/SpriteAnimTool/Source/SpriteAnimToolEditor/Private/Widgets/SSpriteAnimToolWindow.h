#pragma once

#include "CoreMinimal.h"
#include "Misc/Optional.h"
#include "Styling/SlateColor.h"
#include "Widgets/SCompoundWidget.h"

class UTexture2D;
class SSpriteSheetPreview;

class SSpriteAnimToolWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSpriteAnimToolWindow) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FReply HandleApplySplitClicked();
	FReply HandlePlayClicked();
	FReply HandleStopClicked();
	FReply HandlePrevClicked();
	FReply HandleNextClicked();
	FReply HandleNewClicked();
	FReply HandleSaveClicked();
	FReply HandleLoadClicked();

	void HandleTextureChanged(const struct FAssetData& AssetData);
	FString GetSelectedTexturePath() const;
	FText GetFrameCounterText() const;
	void RefreshLoaderPreview();
	void RefreshAnimationPreview();
	EActiveTimerReturnType HandlePlaybackTimer(double InCurrentTime, float InDeltaTime);
	void StopPlayback();

	TSharedRef<class SWidget> BuildLoaderPanel();
	TSharedRef<class SWidget> BuildSettingsPanel();
	TSharedRef<class SWidget> BuildPreviewPanel();
	TSharedRef<class SWidget> BuildFooter();

	int32 Rows = 1;
	int32 Columns = 1;
	float FramesPerSecond = 12.0f;
	bool bLoop = true;
	bool bSplitApplied = false;
	bool bIsPlaying = false;
	int32 CurrentFrame = 0;
	int32 TotalFrames = 1;
	float PlaybackAccumulator = 0.0f;

	TObjectPtr<UTexture2D> SelectedTexture = nullptr;
	TSharedPtr<SSpriteSheetPreview> LoaderPreviewWidget;
	TSharedPtr<SSpriteSheetPreview> AnimationPreviewWidget;
	TWeakPtr<FActiveTimerHandle> PlaybackTimerHandle;

	TSharedPtr<FSlateColorBrush> WindowBackgroundBrush;
	TSharedPtr<FSlateColorBrush> PanelBackgroundBrush;
	TSharedPtr<FSlateColorBrush> FooterBackgroundBrush;
	FButtonStyle SecondaryButtonStyle;
	FButtonStyle PrimaryButtonStyle;
};
