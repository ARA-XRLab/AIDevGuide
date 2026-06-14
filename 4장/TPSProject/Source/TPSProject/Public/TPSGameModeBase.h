// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameModeBase.generated.h"

// TPS 프로젝트의 기본 게임 규칙과 플레이어 시작 흐름을 관리하는 게임 모드 클래스입니다.
UCLASS()
class TPSPROJECT_API ATPSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 게임 모드의 기본 설정을 초기화하는 생성자입니다.
	ATPSGameModeBase();
};
