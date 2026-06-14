// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actors/SpriteAnimActor.h"

#include "Components/StaticMeshComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "UObject/ConstructorHelpers.h"

namespace SpriteAnimActorNames
{
	const FName SpriteSheetParameter(TEXT("SpriteSheet"));
	const FName UVOffsetParameter(TEXT("UVOffset"));
	const FName UVSizeParameter(TEXT("UVSize"));
}

ASpriteAnimActor::ASpriteAnimActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PlaneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
	PlaneMeshComponent->SetupAttachment(SceneRoot);
	PlaneMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	PlaneMeshComponent->SetGenerateOverlapEvents(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMeshFinder.Succeeded())
	{
		PlaneMeshComponent->SetStaticMesh(PlaneMeshFinder.Object);
	}
}

void ASpriteAnimActor::BeginPlay()
{
	Super::BeginPlay();

	bAnimationLoaded = LoadAnimationData();
	if (bAnimationLoaded)
	{
		InitializeMaterial();
		ResetAnimationState();
	}
}

void ASpriteAnimActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bAnimationLoaded || DynamicMaterialInstance == nullptr)
	{
		return;
	}

	AdvanceAnimation(DeltaSeconds);
}

#if WITH_EDITOR
void ASpriteAnimActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (SpriteAnimationMaterial != nullptr)
	{
		InitializeMaterial();
	}
}
#endif

bool ASpriteAnimActor::LoadAnimationData()
{
	AnimationData = FSpriteAnimRuntimeData();
	LoadedSpriteSheet = nullptr;

	const FString FilePath = ResolveAnimationFilePath();
	if (FilePath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("SpriteAnimActor '%s' has no animation data file set."), *GetName());
		return false;
	}

	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *FilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpriteAnimActor '%s' failed to read .sprta file: %s"), *GetName(), *FilePath);
		return false;
	}

	if (!ParseAnimationJson(FileContents, AnimationData))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpriteAnimActor '%s' failed to parse .sprta file: %s"), *GetName(), *FilePath);
		return false;
	}

	if (!AnimationData.ImagePath.IsEmpty())
	{
		LoadedSpriteSheet = LoadObject<UTexture2D>(nullptr, *AnimationData.ImagePath);
	}

	if (LoadedSpriteSheet == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpriteAnimActor '%s' failed to load sprite sheet: %s"), *GetName(), *AnimationData.ImagePath);
		return false;
	}

	return true;
}

bool ASpriteAnimActor::ParseAnimationJson(const FString& FileContents, FSpriteAnimRuntimeData& OutData) const
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return false;
	}

	FString ImagePath;
	double FramesPerSecond = 12.0;
	int32 Rows = 1;
	int32 Columns = 1;
	int32 TotalFrames = 1;
	bool bLoop = true;

	if (!JsonObject->TryGetStringField(TEXT("imagePath"), ImagePath)
		|| !JsonObject->TryGetNumberField(TEXT("rows"), Rows)
		|| !JsonObject->TryGetNumberField(TEXT("columns"), Columns)
		|| !JsonObject->TryGetNumberField(TEXT("totalFrames"), TotalFrames)
		|| !JsonObject->TryGetNumberField(TEXT("fps"), FramesPerSecond)
		|| !JsonObject->TryGetBoolField(TEXT("loop"), bLoop))
	{
		return false;
	}

	OutData.ImagePath = ImagePath;
	OutData.Rows = FMath::Max(1, Rows);
	OutData.Columns = FMath::Max(1, Columns);
	OutData.TotalFrames = FMath::Clamp(TotalFrames, 1, OutData.Rows * OutData.Columns);
	OutData.FramesPerSecond = FMath::Max(1.0f, static_cast<float>(FramesPerSecond));
	OutData.bLoop = bLoop;
	return true;
}

FString ASpriteAnimActor::ResolveAnimationFilePath() const
{
	if (AnimationDataFile.FilePath.IsEmpty())
	{
		return FString();
	}

	if (FPaths::IsRelative(AnimationDataFile.FilePath))
	{
		return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), AnimationDataFile.FilePath);
	}

	return AnimationDataFile.FilePath;
}

void ASpriteAnimActor::InitializeMaterial()
{
	if (SpriteAnimationMaterial == nullptr || PlaneMeshComponent == nullptr)
	{
		return;
	}

	DynamicMaterialInstance = PlaneMeshComponent->CreateDynamicMaterialInstance(0, SpriteAnimationMaterial);
	if (DynamicMaterialInstance != nullptr && LoadedSpriteSheet != nullptr)
	{
		DynamicMaterialInstance->SetTextureParameterValue(SpriteAnimActorNames::SpriteSheetParameter, LoadedSpriteSheet);
	}
}

void ASpriteAnimActor::ApplyFrameToMaterial(int32 FrameIndex)
{
	if (DynamicMaterialInstance == nullptr || LoadedSpriteSheet == nullptr)
	{
		return;
	}

	const int32 Columns = FMath::Max(1, AnimationData.Columns);
	const int32 Rows = FMath::Max(1, AnimationData.Rows);
	const int32 ClampedFrameIndex = FMath::Clamp(FrameIndex, 0, FMath::Max(0, AnimationData.TotalFrames - 1));
	const int32 FrameRow = ClampedFrameIndex / Columns;
	const int32 FrameColumn = ClampedFrameIndex % Columns;

	const FVector UVOffset(
		static_cast<float>(FrameColumn) / static_cast<float>(Columns),
		static_cast<float>(FrameRow) / static_cast<float>(Rows),
		0.0f);
	const FVector UVSize(
		1.0f / static_cast<float>(Columns),
		1.0f / static_cast<float>(Rows),
		0.0f);

	DynamicMaterialInstance->SetVectorParameterValue(SpriteAnimActorNames::UVOffsetParameter, FLinearColor(UVOffset.X, UVOffset.Y, UVOffset.Z, 0.0f));
	DynamicMaterialInstance->SetVectorParameterValue(SpriteAnimActorNames::UVSizeParameter, FLinearColor(UVSize.X, UVSize.Y, UVSize.Z, 0.0f));
}

void ASpriteAnimActor::AdvanceAnimation(float DeltaSeconds)
{
	const float SecondsPerFrame = 1.0f / FMath::Max(AnimationData.FramesPerSecond, 1.0f);
	PlaybackAccumulator += DeltaSeconds;

	while (PlaybackAccumulator >= SecondsPerFrame)
	{
		PlaybackAccumulator -= SecondsPerFrame;

		if (CurrentFrameIndex < AnimationData.TotalFrames - 1)
		{
			++CurrentFrameIndex;
			ApplyFrameToMaterial(CurrentFrameIndex);
			continue;
		}

		if (AnimationData.bLoop)
		{
			CurrentFrameIndex = 0;
			ApplyFrameToMaterial(CurrentFrameIndex);
			continue;
		}

		CurrentFrameIndex = AnimationData.TotalFrames - 1;
		ApplyFrameToMaterial(CurrentFrameIndex);
		SetActorTickEnabled(false);
		break;
	}
}

void ASpriteAnimActor::ResetAnimationState()
{
	SetActorTickEnabled(true);
	PlaybackAccumulator = 0.0f;
	CurrentFrameIndex = 0;
	ApplyFrameToMaterial(CurrentFrameIndex);
}
