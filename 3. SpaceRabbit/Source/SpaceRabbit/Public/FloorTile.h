#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorTile.generated.h"

class UBoxComponent;
class UPaperSpriteComponent;
class AFloorTile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloorTileExitedSignature, AFloorTile*, FloorTile);

UCLASS()
class SPACERABBIT_API AFloorTile : public AActor
{
	GENERATED_BODY()

public:
	AFloorTile();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "FloorTile")
	float GetTileWidth() const { return TileWidth; }

	UFUNCTION(BlueprintPure, Category = "FloorTile")
	FVector GetTopCenterLocation() const;

	UFUNCTION(BlueprintCallable, Category = "FloorTile")
	void SetTileActive(bool bInIsActive);

	UPROPERTY(BlueprintAssignable, Category = "FloorTile")
	FFloorTileExitedSignature OnTileExited;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPaperSpriteComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorTile")
	float TileWidth = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorTile")
	float MoveSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FloorTile")
	float DestroyY = -1500.0f;

private:
	bool bHasExited = false;

	void UpdateTileSize();
	void MoveTile(float DeltaSeconds);
	void UpdateExitState();
};
