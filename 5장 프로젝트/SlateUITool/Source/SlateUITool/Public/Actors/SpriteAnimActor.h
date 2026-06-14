// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpriteAnimActor.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMeshComponent;
class UTexture2D;

USTRUCT()
struct FSpriteAnimRuntimeData
{
	GENERATED_BODY()

	UPROPERTY()
	FString ImagePath;

	UPROPERTY()
	int32 Rows = 1;

	UPROPERTY()
	int32 Columns = 1;

	UPROPERTY()
	int32 TotalFrames = 1;

	UPROPERTY()
	float FramesPerSecond = 12.0f;

	UPROPERTY()
	bool bLoop = true;
};

UCLASS()
class SLATEUITOOL_API ASpriteAnimActor : public AActor
{
	GENERATED_BODY()

public:
	ASpriteAnimActor();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

private:
	bool LoadAnimationData();
	bool ParseAnimationJson(const FString& FileContents, FSpriteAnimRuntimeData& OutData) const;
	FString ResolveAnimationFilePath() const;
	void InitializeMaterial();
	void ApplyFrameToMaterial(int32 FrameIndex);
	void AdvanceAnimation(float DeltaSeconds);
	void ResetAnimationState();

	UPROPERTY(VisibleAnywhere, Category = "Sprite Animation")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Sprite Animation")
	TObjectPtr<UStaticMeshComponent> PlaneMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Sprite Animation", meta = (RelativeToGameDir, FilePathFilter = "Sprite Animation Data (*.sprta)|*.sprta"))
	FFilePath AnimationDataFile;

	UPROPERTY(EditAnywhere, Category = "Sprite Animation")
	TObjectPtr<UMaterialInterface> SpriteAnimationMaterial = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialInstance = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> LoadedSpriteSheet = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Sprite Animation")
	FSpriteAnimRuntimeData AnimationData;

	float PlaybackAccumulator = 0.0f;
	int32 CurrentFrameIndex = 0;
	bool bAnimationLoaded = false;
};
