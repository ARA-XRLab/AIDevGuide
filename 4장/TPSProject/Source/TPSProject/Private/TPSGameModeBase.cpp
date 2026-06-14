// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSGameModeBase.h"

ATPSGameModeBase::ATPSGameModeBase()
{
	// 게임 모드는 매 프레임 처리할 내용이 없으므로 Tick을 비활성화합니다.
	PrimaryActorTick.bCanEverTick = false;

	// 나중에 플레이어 캐릭터를 DefaultPawnClass에 연결할 예정입니다.
}
