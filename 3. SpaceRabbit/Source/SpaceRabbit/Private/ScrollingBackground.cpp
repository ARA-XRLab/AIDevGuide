#include "ScrollingBackground.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "PaperSpriteComponent.h"

AScrollingBackground::AScrollingBackground()
{
	PrimaryActorTick.bCanEverTick = true;

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	SetRootComponent(SpriteComponent);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AScrollingBackground::BeginPlay()
{
	Super::BeginPlay();

	CurrentUOffset = InitialUOffset;
	CreateDynamicMaterial();

	if (DynamicMaterialInstance != nullptr)
	{
		CurrentUOffset = FMath::Fmod(CurrentUOffset, 1.0f);
		if (CurrentUOffset < 0.0f)
		{
			CurrentUOffset += 1.0f;
		}

		DynamicMaterialInstance->SetScalarParameterValue(UOffsetParameterName, CurrentUOffset);
	}
}

void AScrollingBackground::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateBackgroundOffset(DeltaSeconds);
}

void AScrollingBackground::CreateDynamicMaterial()
{
	UMaterialInterface* MaterialToApply = BackgroundMaterial;
	if (MaterialToApply == nullptr)
	{
		MaterialToApply = SpriteComponent != nullptr ? SpriteComponent->GetMaterial(0) : nullptr;
	}

	if (SpriteComponent == nullptr || MaterialToApply == nullptr)
	{
		return;
	}

	DynamicMaterialInstance = SpriteComponent->CreateDynamicMaterialInstance(0, MaterialToApply);
}

void AScrollingBackground::UpdateBackgroundOffset(float DeltaSeconds)
{
	if (DynamicMaterialInstance == nullptr)
	{
		return;
	}

	CurrentUOffset -= ScrollSpeed * DeltaSeconds;
	CurrentUOffset = FMath::Fmod(CurrentUOffset, 1.0f);
	if (CurrentUOffset < 0.0f)
	{
		CurrentUOffset += 1.0f;
	}

	DynamicMaterialInstance->SetScalarParameterValue(UOffsetParameterName, CurrentUOffset);
}
