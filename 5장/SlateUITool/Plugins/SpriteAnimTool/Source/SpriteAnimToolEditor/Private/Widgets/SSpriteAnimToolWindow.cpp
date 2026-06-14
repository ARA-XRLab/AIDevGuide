#include "Widgets/SSpriteAnimToolWindow.h"

#include "AssetRegistry/AssetData.h"
#include "DesktopPlatformModule.h"
#include "Engine/Texture2D.h"
#include "Engine/StreamableManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"
#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "PropertyCustomizationHelpers.h"
#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/SSpriteSheetPreview.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SSpriteAnimToolWindow"

namespace SpriteAnimTool::Colors
{
	static const FLinearColor WindowBackground(0.08f, 0.08f, 0.08f, 1.0f);
	static const FLinearColor PanelBackground(0.11f, 0.11f, 0.11f, 1.0f);
	static const FLinearColor FooterBackground(0.14f, 0.14f, 0.14f, 1.0f);
	static const FLinearColor PreviewBackground(0.93f, 0.93f, 0.93f, 1.0f);
	static const FLinearColor PrimaryButton(0.06f, 0.18f, 0.30f, 1.0f);
	static const FLinearColor PrimaryButtonHovered(0.09f, 0.24f, 0.39f, 1.0f);
	static const FLinearColor SecondaryButton(0.23f, 0.23f, 0.23f, 1.0f);
	static const FLinearColor SecondaryButtonHovered(0.29f, 0.29f, 0.29f, 1.0f);
}

namespace SpriteAnimTool
{
}

void SSpriteAnimToolWindow::Construct(const FArguments& InArgs)
{
	WindowBackgroundBrush = MakeShared<FSlateColorBrush>(SpriteAnimTool::Colors::WindowBackground);
	PanelBackgroundBrush = MakeShared<FSlateColorBrush>(SpriteAnimTool::Colors::PanelBackground);
	FooterBackgroundBrush = MakeShared<FSlateColorBrush>(SpriteAnimTool::Colors::FooterBackground);

	SecondaryButtonStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Button");
	SecondaryButtonStyle.SetNormal(FSlateRoundedBoxBrush(SpriteAnimTool::Colors::SecondaryButton, 4.0f));
	SecondaryButtonStyle.SetHovered(FSlateRoundedBoxBrush(SpriteAnimTool::Colors::SecondaryButtonHovered, 4.0f));
	SecondaryButtonStyle.SetPressed(FSlateRoundedBoxBrush(FLinearColor(0.18f, 0.18f, 0.18f, 1.0f), 4.0f));
	SecondaryButtonStyle.SetNormalForeground(FLinearColor::White);
	SecondaryButtonStyle.SetHoveredForeground(FLinearColor::White);
	SecondaryButtonStyle.SetPressedForeground(FLinearColor::White);

	PrimaryButtonStyle = SecondaryButtonStyle;
	PrimaryButtonStyle.SetNormal(FSlateRoundedBoxBrush(SpriteAnimTool::Colors::PrimaryButton, 4.0f));
	PrimaryButtonStyle.SetHovered(FSlateRoundedBoxBrush(SpriteAnimTool::Colors::PrimaryButtonHovered, 4.0f));
	PrimaryButtonStyle.SetPressed(FSlateRoundedBoxBrush(FLinearColor(0.05f, 0.14f, 0.24f, 1.0f), 4.0f));

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(WindowBackgroundBrush.Get())
		.Padding(0.0f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 12.0f, 0.0f, 10.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("WindowTitle", "2D Sprite Animation Tool"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
				.Justification(ETextJustify::Center)
				.ColorAndOpacity(FLinearColor(0.78f, 0.78f, 0.78f, 1.0f))
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(6.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.FillWidth(1.15f)
				.Padding(0.0f, 0.0f, 4.0f, 0.0f)
				[
					BuildLoaderPanel()
				]

				+ SHorizontalBox::Slot()
				.FillWidth(0.98f)
				.Padding(4.0f, 0.0f)
				[
					BuildSettingsPanel()
				]

				+ SHorizontalBox::Slot()
				.FillWidth(1.15f)
				.Padding(4.0f, 0.0f, 0.0f, 0.0f)
				[
					BuildPreviewPanel()
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(4.0f, 4.0f, 4.0f, 4.0f)
			[
				BuildFooter()
			]
		]
	];
}

TSharedRef<SWidget> SSpriteAnimToolWindow::BuildLoaderPanel()
{
	return SNew(SBorder)
		.BorderImage(PanelBackgroundBrush.Get())
		.Padding(14.0f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("LoaderPanelTitle", "Sprite Sheet Loader"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(0.0f, 12.0f, 0.0f, 10.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("WhiteBrush"))
				.BorderBackgroundColor(SpriteAnimTool::Colors::PreviewBackground)
				.Padding(6.0f)
				[
					SNew(SBox)
					.MinDesiredHeight(380.0f)
					.MinDesiredWidth(320.0f)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SAssignNew(LoaderPreviewWidget, SSpriteSheetPreview)
					]
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 6.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SelectImageLabel", "Select Image"))
				.ColorAndOpacity(FLinearColor(0.74f, 0.74f, 0.74f, 1.0f))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UTexture2D::StaticClass())
				.ObjectPath(this, &SSpriteAnimToolWindow::GetSelectedTexturePath)
				.OnObjectChanged(this, &SSpriteAnimToolWindow::HandleTextureChanged)
				.DisplayThumbnail(true)
				.DisplayBrowse(true)
				.DisplayUseSelected(true)
				.EnableContentPicker(true)
			]
		];
}

TSharedRef<SWidget> SSpriteAnimToolWindow::BuildSettingsPanel()
{
	return SNew(SBorder)
		.BorderImage(PanelBackgroundBrush.Get())
		.Padding(14.0f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SettingsPanelTitle", "Frame Settings"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 16.0f, 0.0f, 0.0f)
			[
				SNew(SGridPanel)
				.FillColumn(0, 0.46f)
				.FillColumn(1, 0.54f)

				+ SGridPanel::Slot(0, 0)
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 10.0f, 8.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("RowsLabel", "Rows"))
				]
				+ SGridPanel::Slot(1, 0)
				.Padding(0.0f, 0.0f, 0.0f, 8.0f)
				[
					SNew(SNumericEntryBox<int32>)
					.MinValue(1)
					.Value_Lambda([this]() { return Rows; })
					.OnValueChanged_Lambda([this](int32 InValue)
					{
						Rows = FMath::Max(1, InValue);
						TotalFrames = FMath::Clamp(TotalFrames, 1, Rows * Columns);
					})
				]

				+ SGridPanel::Slot(0, 1)
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 10.0f, 8.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ColumnsLabel", "Columns"))
				]
				+ SGridPanel::Slot(1, 1)
				.Padding(0.0f, 0.0f, 0.0f, 8.0f)
				[
					SNew(SNumericEntryBox<int32>)
					.MinValue(1)
					.Value_Lambda([this]() { return Columns; })
					.OnValueChanged_Lambda([this](int32 InValue)
					{
						Columns = FMath::Max(1, InValue);
						TotalFrames = FMath::Clamp(TotalFrames, 1, Rows * Columns);
					})
				]

				+ SGridPanel::Slot(0, 2)
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 10.0f, 8.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("TotalFramesLabel", "Total Frames"))
				]
				+ SGridPanel::Slot(1, 2)
				.Padding(0.0f, 0.0f, 0.0f, 8.0f)
				[
					SNew(SNumericEntryBox<int32>)
					.MinValue(1)
					.MaxValue_Lambda([this]() { return FMath::Max(1, Rows * Columns); })
					.Value_Lambda([this]() { return FMath::Clamp(TotalFrames, 1, Rows * Columns); })
					.OnValueChanged_Lambda([this](int32 InValue)
					{
						TotalFrames = FMath::Clamp(InValue, 1, Rows * Columns);
					})
				]

				+ SGridPanel::Slot(0, 3)
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 10.0f, 8.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("FPSLabel", "FPS"))
				]
				+ SGridPanel::Slot(1, 3)
				.Padding(0.0f, 0.0f, 0.0f, 8.0f)
				[
					SNew(SNumericEntryBox<float>)
					.MinValue(1.0f)
					.MinSliderValue(1.0f)
					.MaxSliderValue(60.0f)
					.Value_Lambda([this]() { return FramesPerSecond; })
					.OnValueChanged_Lambda([this](float InValue)
					{
						FramesPerSecond = FMath::Max(1.0f, InValue);
					})
				]

				+ SGridPanel::Slot(0, 4)
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 10.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("LoopLabel", "Loop"))
				]
				+ SGridPanel::Slot(1, 4)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
					.IsChecked_Lambda([this]() { return bLoop ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
					.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState)
					{
						bLoop = (NewState == ECheckBoxState::Checked);
					})
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 18.0f, 0.0f, 0.0f)
			[
				SNew(SButton)
				.ButtonStyle(&PrimaryButtonStyle)
				.OnClicked(this, &SSpriteAnimToolWindow::HandleApplySplitClicked)
				.ContentPadding(FMargin(18.0f, 7.0f))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ApplySplitButton", "Apply Split"))
					.Justification(ETextJustify::Center)
				]
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SSpacer)
			]
		];
}

TSharedRef<SWidget> SSpriteAnimToolWindow::BuildPreviewPanel()
{
	return SNew(SBorder)
		.BorderImage(PanelBackgroundBrush.Get())
		.Padding(14.0f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("PreviewPanelTitle", "Animation Preview"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
			]

			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(0.0f, 12.0f, 0.0f, 10.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("WhiteBrush"))
				.BorderBackgroundColor(SpriteAnimTool::Colors::PreviewBackground)
				.Padding(8.0f)
				[
					SNew(SBox)
					.MinDesiredHeight(410.0f)
					.MinDesiredWidth(320.0f)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SAssignNew(AnimationPreviewWidget, SSpriteSheetPreview)
					]
				]
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(0.0f, 0.0f, 0.0f, 10.0f)
			[
				SNew(STextBlock)
				.Text(this, &SSpriteAnimToolWindow::GetFrameCounterText)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 8.0f, 0.0f)
				[
					SNew(SButton)
					.ButtonStyle(&SecondaryButtonStyle)
					.OnClicked(this, &SSpriteAnimToolWindow::HandlePlayClicked)
					.ContentPadding(FMargin(18.0f, 6.0f))
					[
						SNew(STextBlock)
						.Text(LOCTEXT("PlayButton", "Play"))
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 8.0f, 0.0f)
				[
					SNew(SButton)
					.ButtonStyle(&SecondaryButtonStyle)
					.OnClicked(this, &SSpriteAnimToolWindow::HandleStopClicked)
					.ContentPadding(FMargin(18.0f, 6.0f))
					[
						SNew(STextBlock)
						.Text(LOCTEXT("StopButton", "Stop"))
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 8.0f, 0.0f)
				[
					SNew(SButton)
					.ButtonStyle(&SecondaryButtonStyle)
					.OnClicked(this, &SSpriteAnimToolWindow::HandlePrevClicked)
					.ContentPadding(FMargin(18.0f, 6.0f))
					[
						SNew(STextBlock)
						.Text(LOCTEXT("PrevButton", "Prev"))
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.ButtonStyle(&SecondaryButtonStyle)
					.OnClicked(this, &SSpriteAnimToolWindow::HandleNextClicked)
					.ContentPadding(FMargin(18.0f, 6.0f))
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NextButton", "Next"))
					]
				]
			]
		];
}

TSharedRef<SWidget> SSpriteAnimToolWindow::BuildFooter()
{
	return SNew(SBorder)
		.BorderImage(FooterBackgroundBrush.Get())
		.Padding(12.0f, 10.0f)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSpacer)
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SButton)
				.ButtonStyle(&SecondaryButtonStyle)
				.OnClicked(this, &SSpriteAnimToolWindow::HandleNewClicked)
				.ContentPadding(FMargin(18.0f, 5.0f))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("NewButton", "+ New"))
				]
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 10.0f, 0.0f)
			[
				SNew(SButton)
				.ButtonStyle(&SecondaryButtonStyle)
				.OnClicked(this, &SSpriteAnimToolWindow::HandleSaveClicked)
				.ContentPadding(FMargin(18.0f, 5.0f))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SaveButton", "Save"))
				]
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.ButtonStyle(&SecondaryButtonStyle)
				.OnClicked(this, &SSpriteAnimToolWindow::HandleLoadClicked)
				.ContentPadding(FMargin(18.0f, 5.0f))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("LoadButton", "Load"))
				]
			]
		];
}

FReply SSpriteAnimToolWindow::HandleApplySplitClicked()
{
	TotalFrames = FMath::Clamp(TotalFrames, 1, Rows * Columns);
	CurrentFrame = 0;
	bSplitApplied = true;
	RefreshLoaderPreview();
	RefreshAnimationPreview();
	return FReply::Handled();
}

FReply SSpriteAnimToolWindow::HandlePlayClicked()
{
	if (!bSplitApplied || SelectedTexture == nullptr || TotalFrames <= 0)
	{
		return FReply::Handled();
	}

	bIsPlaying = true;
	PlaybackAccumulator = 0.0f;

	if (!PlaybackTimerHandle.IsValid())
	{
		PlaybackTimerHandle = RegisterActiveTimer(
			0.0f,
			FWidgetActiveTimerDelegate::CreateSP(this, &SSpriteAnimToolWindow::HandlePlaybackTimer));
	}

	return FReply::Handled();
}

FReply SSpriteAnimToolWindow::HandleStopClicked()
{
	StopPlayback();
	return FReply::Handled();
}

FReply SSpriteAnimToolWindow::HandlePrevClicked()
{
	StopPlayback();

	if (bSplitApplied && TotalFrames > 0 && CurrentFrame > 0)
	{
		--CurrentFrame;
		RefreshAnimationPreview();
	}

	return FReply::Handled();
}

FReply SSpriteAnimToolWindow::HandleNextClicked()
{
	StopPlayback();

	if (bSplitApplied && TotalFrames > 0 && CurrentFrame < TotalFrames - 1)
	{
		++CurrentFrame;
		RefreshAnimationPreview();
	}

	return FReply::Handled();
}

FReply SSpriteAnimToolWindow::HandleNewClicked()
{
	const EAppReturnType::Type DialogResult = FMessageDialog::Open(
		EAppMsgType::YesNo,
		LOCTEXT("ResetConfirmMessage", "정말 초기화할까요?"));

	if (DialogResult != EAppReturnType::Yes)
	{
		return FReply::Handled();
	}

	StopPlayback();
	bSplitApplied = false;
	Rows = 1;
	Columns = 1;
	FramesPerSecond = 12.0f;
	bLoop = true;
	CurrentFrame = 0;
	TotalFrames = 1;
	SelectedTexture = nullptr;
	RefreshLoaderPreview();
	RefreshAnimationPreview();
	return FReply::Handled();
}

FReply SSpriteAnimToolWindow::HandleSaveClicked()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform == nullptr)
	{
		FNotificationInfo NotificationInfo(LOCTEXT("SaveUnavailable", "저장 창을 열 수 없습니다."));
		NotificationInfo.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return FReply::Handled();
	}

	void* ParentWindowHandle = nullptr;
	const TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindBestParentWindowForDialogs(nullptr);
	if (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
	{
		ParentWindowHandle = ParentWindow->GetNativeWindow()->GetOSWindowHandle();
	}

	const FString DefaultDirectory = FPaths::ProjectSavedDir();
	const FString DefaultFileName = TEXT("SpriteAnimationData");
	const FString FileTypes = TEXT("Sprite Animation Data (*.sprta)|*.sprta|JSON Files (*.json)|*.json");
	TArray<FString> SelectedFiles;

	const bool bSavedFileSelected = DesktopPlatform->SaveFileDialog(
		ParentWindowHandle,
		TEXT("Save Sprite Animation Data"),
		DefaultDirectory,
		DefaultFileName,
		FileTypes,
		EFileDialogFlags::None,
		SelectedFiles);

	if (!bSavedFileSelected || SelectedFiles.IsEmpty())
	{
		return FReply::Handled();
	}

	const TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(TEXT("imagePath"), GetSelectedTexturePath());
	JsonObject->SetNumberField(TEXT("rows"), Rows);
	JsonObject->SetNumberField(TEXT("columns"), Columns);
	JsonObject->SetNumberField(TEXT("totalFrames"), TotalFrames);
	JsonObject->SetNumberField(TEXT("fps"), FramesPerSecond);
	JsonObject->SetBoolField(TEXT("loop"), bLoop);
	FString OutputJson;
	if (!FJsonSerializer::Serialize(JsonObject, TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&OutputJson)))
	{
		FNotificationInfo NotificationInfo(LOCTEXT("SaveSerializeFailed", "저장 실패: JSON 생성에 실패했습니다."));
		NotificationInfo.ExpireDuration = 4.0f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return FReply::Handled();
	}

	FString SavePath = SelectedFiles[0];
	if (FPaths::GetExtension(SavePath).IsEmpty())
	{
		SavePath += TEXT(".sprta");
	}

	if (!FFileHelper::SaveStringToFile(OutputJson, *SavePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		FNotificationInfo NotificationInfo(LOCTEXT("SaveWriteFailed", "저장 실패: 파일을 쓸 수 없습니다."));
		NotificationInfo.ExpireDuration = 4.0f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return FReply::Handled();
	}

	FNotificationInfo NotificationInfo(LOCTEXT("SaveSucceeded", "저장 완료!"));
	NotificationInfo.SubText = FText::FromString(FPaths::GetCleanFilename(SavePath));
	NotificationInfo.ExpireDuration = 3.0f;
	NotificationInfo.bUseSuccessFailIcons = true;
	FSlateNotificationManager::Get().AddNotification(NotificationInfo);

	return FReply::Handled();
}

FReply SSpriteAnimToolWindow::HandleLoadClicked()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform == nullptr)
	{
		FNotificationInfo NotificationInfo(LOCTEXT("LoadUnavailable", "불러오기 창을 열 수 없습니다."));
		NotificationInfo.ExpireDuration = 3.0f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return FReply::Handled();
	}

	void* ParentWindowHandle = nullptr;
	const TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindBestParentWindowForDialogs(nullptr);
	if (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
	{
		ParentWindowHandle = ParentWindow->GetNativeWindow()->GetOSWindowHandle();
	}

	const FString DefaultDirectory = FPaths::ProjectSavedDir();
	const FString FileTypes = TEXT("Sprite Animation Data (*.sprta)|*.sprta|JSON Files (*.json)|*.json");
	TArray<FString> SelectedFiles;

	const bool bOpenFileSelected = DesktopPlatform->OpenFileDialog(
		ParentWindowHandle,
		TEXT("Load Sprite Animation Data"),
		DefaultDirectory,
		TEXT(""),
		FileTypes,
		EFileDialogFlags::None,
		SelectedFiles);

	if (!bOpenFileSelected || SelectedFiles.IsEmpty())
	{
		return FReply::Handled();
	}

	FString InputJson;
	if (!FFileHelper::LoadFileToString(InputJson, *SelectedFiles[0]))
	{
		FNotificationInfo NotificationInfo(LOCTEXT("LoadReadFailed", "불러오기 실패: 파일을 읽을 수 없습니다."));
		NotificationInfo.ExpireDuration = 4.0f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return FReply::Handled();
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(InputJson);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		FNotificationInfo NotificationInfo(LOCTEXT("LoadParseFailed", "불러오기 실패: JSON 형식이 올바르지 않습니다."));
		NotificationInfo.ExpireDuration = 4.0f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return FReply::Handled();
	}

	FString ImagePath;
	double LoadedFPS = 12.0;
	int32 LoadedRows = 1;
	int32 LoadedColumns = 1;
	int32 LoadedTotalFrames = 1;
	bool bLoadedLoop = true;

	if (!JsonObject->TryGetStringField(TEXT("imagePath"), ImagePath)
		|| !JsonObject->TryGetNumberField(TEXT("rows"), LoadedRows)
		|| !JsonObject->TryGetNumberField(TEXT("columns"), LoadedColumns)
		|| !JsonObject->TryGetNumberField(TEXT("totalFrames"), LoadedTotalFrames)
		|| !JsonObject->TryGetNumberField(TEXT("fps"), LoadedFPS)
		|| !JsonObject->TryGetBoolField(TEXT("loop"), bLoadedLoop))
	{
		FNotificationInfo NotificationInfo(LOCTEXT("LoadMissingFields", "불러오기 실패: 필요한 데이터가 부족합니다."));
		NotificationInfo.ExpireDuration = 4.0f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return FReply::Handled();
	}

	UTexture2D* LoadedTexture = nullptr;
	if (!ImagePath.IsEmpty())
	{
		LoadedTexture = LoadObject<UTexture2D>(nullptr, *ImagePath);
	}

	if (!ImagePath.IsEmpty() && LoadedTexture == nullptr)
	{
		FNotificationInfo NotificationInfo(LOCTEXT("LoadTextureFailed", "불러오기 실패: 저장된 이미지를 찾을 수 없습니다."));
		NotificationInfo.SubText = FText::FromString(ImagePath);
		NotificationInfo.ExpireDuration = 4.0f;
		FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		return FReply::Handled();
	}

	StopPlayback();

	Rows = FMath::Max(1, LoadedRows);
	Columns = FMath::Max(1, LoadedColumns);
	TotalFrames = FMath::Clamp(LoadedTotalFrames, 1, Rows * Columns);
	FramesPerSecond = FMath::Max(1.0f, static_cast<float>(LoadedFPS));
	bLoop = bLoadedLoop;
	CurrentFrame = 0;
	SelectedTexture = LoadedTexture;
	bSplitApplied = (SelectedTexture != nullptr);

	RefreshLoaderPreview();
	RefreshAnimationPreview();

	FNotificationInfo NotificationInfo(LOCTEXT("LoadSucceeded", "불러오기 완료!"));
	NotificationInfo.SubText = FText::FromString(FPaths::GetCleanFilename(SelectedFiles[0]));
	NotificationInfo.ExpireDuration = 3.0f;
	NotificationInfo.bUseSuccessFailIcons = true;
	FSlateNotificationManager::Get().AddNotification(NotificationInfo);

	return FReply::Handled();
}

void SSpriteAnimToolWindow::HandleTextureChanged(const FAssetData& AssetData)
{
	StopPlayback();
	SelectedTexture = Cast<UTexture2D>(AssetData.GetAsset());
	bSplitApplied = false;
	CurrentFrame = 0;

	if (LoaderPreviewWidget.IsValid())
	{
		LoaderPreviewWidget->SetTexture(SelectedTexture);
	}

	if (AnimationPreviewWidget.IsValid())
	{
		AnimationPreviewWidget->SetTexture(SelectedTexture);
		AnimationPreviewWidget->ClearFramePreview();
	}

	RefreshLoaderPreview();
}

FString SSpriteAnimToolWindow::GetSelectedTexturePath() const
{
	return SelectedTexture ? SelectedTexture->GetPathName() : FString();
}

FText SSpriteAnimToolWindow::GetFrameCounterText() const
{
	const int32 DisplayTotalFrames = FMath::Max(TotalFrames, 0);
	return FText::Format(
		LOCTEXT("FrameCounter", "Frame: {0} / {1}"),
		FText::AsNumber(bSplitApplied && DisplayTotalFrames > 0 ? CurrentFrame + 1 : 0),
		FText::AsNumber(DisplayTotalFrames));
}

void SSpriteAnimToolWindow::RefreshLoaderPreview()
{
	if (!LoaderPreviewWidget.IsValid())
	{
		return;
	}

	LoaderPreviewWidget->SetTexture(SelectedTexture);

	if (SelectedTexture != nullptr && bSplitApplied)
	{
		LoaderPreviewWidget->SetGridOverlay(Rows, Columns, TotalFrames);
	}
	else
	{
		LoaderPreviewWidget->ClearGridOverlay();
	}
}

void SSpriteAnimToolWindow::RefreshAnimationPreview()
{
	if (!AnimationPreviewWidget.IsValid())
	{
		return;
	}

	AnimationPreviewWidget->SetTexture(SelectedTexture);

	if (SelectedTexture != nullptr && bSplitApplied)
	{
		AnimationPreviewWidget->SetFramePreview(Rows, Columns, CurrentFrame);
	}
	else
	{
		AnimationPreviewWidget->ClearFramePreview();
	}
}

EActiveTimerReturnType SSpriteAnimToolWindow::HandlePlaybackTimer(double InCurrentTime, float InDeltaTime)
{
	if (!bIsPlaying || !bSplitApplied || SelectedTexture == nullptr || TotalFrames <= 0)
	{
		PlaybackTimerHandle.Reset();
		return EActiveTimerReturnType::Stop;
	}

	const float SecondsPerFrame = 1.0f / FMath::Max(FramesPerSecond, 1.0f);
	PlaybackAccumulator += InDeltaTime;

	bool bFrameChanged = false;

	while (PlaybackAccumulator >= SecondsPerFrame)
	{
		PlaybackAccumulator -= SecondsPerFrame;

		if (CurrentFrame < TotalFrames - 1)
		{
			++CurrentFrame;
			bFrameChanged = true;
			continue;
		}

		if (bLoop)
		{
			CurrentFrame = 0;
			bFrameChanged = true;
			continue;
		}

		StopPlayback();
		RefreshAnimationPreview();
		PlaybackTimerHandle.Reset();
		return EActiveTimerReturnType::Stop;
	}

	if (bFrameChanged)
	{
		RefreshAnimationPreview();
	}

	return EActiveTimerReturnType::Continue;
}

void SSpriteAnimToolWindow::StopPlayback()
{
	bIsPlaying = false;
	PlaybackAccumulator = 0.0f;
}

#undef LOCTEXT_NAMESPACE
