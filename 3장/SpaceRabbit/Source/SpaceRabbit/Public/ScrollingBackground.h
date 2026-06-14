#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScrollingBackground.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class UPaperSpriteComponent;

UCLASS()
class SPACERABBIT_API AScrollingBackground : public AActor
{
	GENERATED_BODY()

public:
	AScrollingBackground();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPaperSpriteComponent> SpriteComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Background")
	TObjectPtr<UMaterialInterface> BackgroundMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Background")
	float ScrollSpeed = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Background")
	float InitialUOffset = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Background")
	FName UOffsetParameterName = TEXT("UOffset");

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialInstance;

	float CurrentUOffset = 0.0f;

	void CreateDynamicMaterial();
	void UpdateBackgroundOffset(float DeltaSeconds);
};
