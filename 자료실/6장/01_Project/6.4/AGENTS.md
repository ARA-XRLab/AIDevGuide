# SpaceShooter 에이전트 노트

## 프로젝트 요약
- Unreal Engine 5.7 C++ 프로젝트 (모듈: SpaceShooter), UMG UI와 Enhanced Input 사용.
- 핵심 플레이: 플레이어 폰이 풀링된 총알 발사, 적이 스폰되어 플레이어 쪽으로 이동하며 연사.
- 에셋은 Content/Shooting 아래에 있으며, 생성자에서 하드코딩된 경로로 로드됨.

## 주요 경로
- 게임 모듈: Source/SpaceShooter
- 퍼블릭 헤더: Source/SpaceShooter/Public
- 프라이빗 구현: Source/SpaceShooter/Private
- 프로젝트 파일: SpaceShooter.uproject
- 솔루션: SpaceShooter.sln
- 에셋: Content/Shooting

## 런타임 흐름(개요)
- AShootingGameMode
  - BeginPlay에서 UMainWidget 생성/표시.
  - Score/HighScore 관리, GConfig로 GameUserSettings.ini에 하이스코어 저장.
  - 플레이어 사망 시 UGameOverWidget 표시, 입력 모드를 UI 전용으로 전환.
- APlayerPawn
  - Enhanced Input 바인딩(IMC_Player, IA_Move, IA_Fire).
  - 총알 풀(ABulletActor)과 FireInterval 타이머.
  - HP + 무적 시간(깜빡임 + 머티리얼 파라미터).
- AEnemyManagerActor
  - 이름에 "SpawnPoint"가 포함된 액터를 찾아 타이머로 적 스폰.
- AEnemyActor
  - 플레이어 방향으로 이동, 근접 시 정지하고 AEnemyBulletActor로 연사.
  - 플레이어와 오버랩 시 피해 적용, HP <= 0이면 게임오버 처리.
- ABulletActor (플레이어 총알)
  - 적과 오버랩: 점수 증가, 적 파괴, 풀로 반환.
- AEnemyBulletActor
  - 플레이어와 오버랩: 피해 적용, 총알 비활성화.
- ADestroyZoneActor
  - 총알 정리(풀 반환) 또는 기타 액터 파괴.

## 프로젝트 규칙/주의점
- 충돌 프로필: Player, Enemy, PlayerBullet, EnemyBullet, DestroyZone. 프로젝트 설정에 존재해야 함.
- 에셋 경로는 ConstructorHelpers::FObjectFinder로 하드코딩됨. 이동 시 경로 업데이트 필요.
- 플레이어 총알 풀링은 비활성화 후 ReturnBulletToPool로 반환하는 흐름 전제.
- EnemyManager는 이름에 "SpawnPoint" 포함 여부로 스폰 지점 탐색. 네이밍 규칙 유지 필요.
- UI 위젯은 BindWidget 이름이 위젯 블루프린트와 일치해야 함.

## 빌드/실행
- Unreal Engine 5.7에서 SpaceShooter.uproject 열어 빌드/플레이.
- 또는 Visual Studio에서 SpaceShooter.sln을 열고 SpaceShooter 타깃 빌드.

## 자주 하는 작업
- 입력 추가:
  - 에디터에서 InputAction/MappingContext 생성, IMC_Player에 추가.
  - APlayerPawn::SetupPlayerInputComponent에서 바인딩.
- 적 타입 추가:
  - Source/SpaceShooter에 새 액터 클래스 생성.
  - EnemyFactory 업데이트 또는 AEnemyManagerActor에 스포너 추가.
- UI 요소 추가:
  - 위젯 블루프린트와 UMainWidget/UGameOverWidget 바인딩 갱신.

## 스모크 테스트 체크리스트
- PIE 실행: 플레이어 이동/사격 가능, 적 피격 시 점수 증가.
- 적이 스폰 지점에서 생성되고 근접 시 사격.
- 플레이어 HP UI 업데이트, 사망 시 게임오버 화면 표시.
- 레벨 재시작 후 하이스코어 유지.

## 인코딩 규칙
- 마크다운 파일은 UTF-8 (BOM 없음)으로 저장.