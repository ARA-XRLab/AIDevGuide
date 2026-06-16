# 프로젝트명: Space Rabbit

## 1. 프로젝트 개요
- 게임명: Space Rabbit
- 장르: 2D 횡스크롤 무한 러닝 게임
- 엔진: Unreal Engine (Paper2D)
- 언어: C++ (Blueprint 최소화)
- 개발 규모: 1인 개발, 2주 프로토타입
- 타겟 플랫폼: PC

## 2. 핵심 컨셉
### 한 줄 요약
"우주복을 입은 귀여운 토끼가 끝없이 펼쳐진 우주 정거장을 달리며
장애물을 피하고 코인을 모으는 중독성 있는 러닝 게임"

### 핵심 재미 요소
1. 긴장감 있는 회피 액션 - 점프 타이밍의 쾌감
2. 수집의 재미 - 코인 획득 시 시각 피드백
3. 원터치 조작 - 간단하지만 타이밍이 중요

### 레퍼런스
- 쿠키런: 무한 러닝 + 수집 시스템
- 플래피 버드: 단순하지만 중독성 있는 조작

## 3. 게임 규칙

### 기본 규칙
1. 게임 시작 즉시 플레이 시작 (타이틀/준비 화면 없음)
2. 플레이어(토끼)는 제자리, 배경이 오른쪽에서 왼쪽으로 자동 스크롤
3. 스페이스바를 누르면 점프 (바닥에 있을 때만)
4. 2단 점프 가능

### 게임오버 조건
1. 장애물(바위)에 충돌하는 경우 → 즉시 게임오버
2. 로켓에 충돌하는 경우 → 즉시 게임오버
3. 바닥 구멍에 추락하는 경우 → 즉시 게임오버

### 점수 시스템
- 코인 획득: +1점

### 게임 흐름
플레이(Playing) → 게임오버(GameOver) → 재시작/종료

## 4. 게임 오브젝트

### 플레이어 (Space Rabbit)
- 클래스명: ASpaceRabbitCharacter
- 부모 클래스: APawn
- 역할: 플레이어가 조작하는 주인공 캐릭터

컴포넌트:
- UCapsuleComponent: 물리 충돌 처리
- UPaperFlipbookComponent: 토끼 애니메이션

주요 변수:
- float JumpForce = 400.0f (점프력)
- bool bIsJumping = false (점프 중 여부)
- bool bIsGrounded = true (바닥 접촉 여부)
- int32 JumpCount = 0 (현재 점프 횟수, 최대 2)

동작:
1. 자동으로 오른쪽 방향 이동 (실제로는 배경이 왼쪽으로)
2. 스페이스바 입력 시 점프 (2단 점프 가능)
3. 장애물/로켓 충돌 시 게임오버 이벤트 발생
4. 바닥 추락 시 게임오버 이벤트 발생
---

### 코인 (Coin)
- 클래스명: ACoin
- 부모 클래스: AActor
- 역할: 플레이어가 수집하는 점수 아이템

컴포넌트:
- UPaperSpriteComponent: 코인 스프라이트
- USphereComponent: 충돌 감지 (Overlap)

주요 변수:
- int32 CoinValue = 1

동작:
1. 바닥 발판과 함께 왼쪽으로 이동
2. 플레이어와 Overlap 시 점수 추가 후 Destroy

---
### 장애물 (Obstacle)
- 클래스명: AObstacle
- 부모 클래스: AActor
- 역할: 충돌 시 즉시 게임오버를 유발하는 위협 요소

컴포넌트:
- UPaperSpriteComponent: 바위 스프라이트
- UBoxComponent: 충돌 감지 (Overlap)

동작:
1. 바닥 발판 위에 스폰
2. 발판과 함께 왼쪽으로 이동
3. 플레이어와 충돌 시 게임오버
---

### 로켓 (Rocket)
- 클래스명: ARocket
- 부모 클래스: AActor
- 역할: 상공에서 날아오는 위협 요소

컴포넌트:
- UPaperSpriteComponent: 로켓 스프라이트
- UBoxComponent: 충돌 감지

주요 변수:
- float RocketSpeed = 500.0f

동작:
1. 화면 오른쪽 상단에서 스폰
2. 왼쪽으로 직선 이동
3. 플레이어와 충돌 시 게임오버

---
### 바닥 발판 (FloorTile)
- 클래스명: AFloorTile
- 부모 클래스: AActor
- 역할: 플레이어가 딛고 달리는 바닥

컴포넌트:
- UPaperSpriteComponent: 발판 스프라이트
- UBoxComponent: 물리 충돌

주요 변수:
- float TileWidth = 500.0f
- float MoveSpeed = 300.0f

동작:
1. 오른쪽 끝에서 스폰되어 왼쪽으로 이동
2. 화면 밖으로 나가면 오른쪽 끝으로 재배치 (무한 스크롤)
3. 발판 사이 간격이 "구멍"이 됨

## 5. 게임플레이 루프

### 게임 상태 (State)
| 상태 | 설명 | UI |
|------|------|-----|
| Playing | 게임 진행 중 | HUD (점수) |
| GameOver | 게임 종료 | 최종 점수, 재시작/종료 버튼 |

### 상태 전이 조건
- 게임 시작 → Playing (즉시)
- Playing → GameOver: 장애물/로켓 충돌 또는 추락
- GameOver → Playing: 재시작 버튼 클릭

### 핵심 루프 (Playing 상태)
달리기(자동) → 위협 인식 → 점프/피격 → 코인 수집 → 반복

### 재미/긴장감 분석
| 요소 | 효과 | 구현 방법 |
|------|------|----------|
| 자동 달리기 | 긴장감 유지 | 쉴 틈 없는 판단 요구 |
| 즉사 시스템 | 높은 긴장감 | 실수 하나가 게임오버 |
| 2단 점프 | 전략적 선택 | 타이밍과 높이 조절 |
| 코인 수집 | 보상감 | 점수 피드백 |

## 6. UI/UX 설계

### 인게임 HUD (Playing)
- 좌측 상단: 점수 ("Score : " + 숫자)

### 게임오버 화면 (GameOver)
- 반투명 어두운 오버레이
- 화면 중앙: "GAME OVER"
- 중앙 하단: 최종 점수
- 버튼: [RESTART] [QUIT]

## 7. 기술 명세

### 클래스 구조
| 클래스명 | 부모 클래스 | 역할 |
|----------|-------------|------|
| ASpaceRabbitGameMode | AGameModeBase | 게임 상태 관리 |
| ASpaceRabbitCharacter | APawn | 플레이어 캐릭터 |
| ACoin | AActor | 수집 아이템 |
| AObstacle | AActor | 장애물 |
| ARocket | AActor | 로켓 |
| AFloorTile | AActor | 바닥 발판 |
| AFloorManager | AActor | 발판 스폰/관리 |
| USpaceRabbitHUD | UUserWidget | 인게임 HUD |
| UGameOverWidget | UUserWidget | 게임오버 UI |

### 개발 마일스톤
| 단계 | 버전 | 목표 | 기간 |
|------|------|------|------|
| 1 | 프로토타입 | 기본 도형으로 핵심 메커니즘 | 3일 |
| 2 | 알파 | 게임플레이 루프, UI 완성 | 4일 |
| 3 | 베타 | Paper2D 리소스, 최적화 | 3일 |

### 블루프린트와 에셋 수정 원칙
- 블루프린트는 텍스트 diff가 약하므로, C++에서 대체 가능한 로직은 우선 C++로 이동한다.
- 블루프린트 수정이 필요하면 다음을 함께 기록한다.
  - 어떤 에셋을 수정했는지
  - 왜 C++가 아닌 블루프린트에서 처리했는지
  - 런타임에서 기대되는 변경점이 무엇인지
- 에셋 이름은 역할이 드러나게 유지한다.
  - 예: `BP_SpaceRabbitCharacter`, `BP_SpaceRabbitGameMode`, `IA_Jump`, `IMC_Player`

### 빌드 및 실행 기준
- 솔루션 파일: `SpaceRabbit.sln`
- 프로젝트 파일: `SpaceRabbit.uproject`
- 새 C++ 클래스 추가 후에는 프로젝트 파일 재생성이 필요할 수 있다.
- 일반적인 확인 순서:
  1. 컴파일
  2. 에디터에서 맵 로드
  3. 입력 동작 확인
  4. 충돌/오버랩 확인
  5. 게임오버 및 재시작 흐름 확인

### 검증 원칙
- 코드 변경 후 최소한 다음 영향을 점검한다.
  - 빌드 성공 여부
  - 기본 맵 로드 가능 여부
  - 플레이어 입력 동작 여부
  - 관련 액터의 충돌/오버랩 이벤트 정상 동작 여부
- 테스트를 못 했다면 이유를 명확히 남긴다.
- UE 로그가 중요하므로, 실패 시 컴파일 에러보다 먼저 경고와 런타임 로그 원인을 함께 본다.

### 작업 시 주의사항
- 사용자가 만든 기존 변경 사항을 임의로 되돌리지 않는다.
- `.uasset`, `.umap`은 바이너리 파일이므로 삭제/대체 시 영향 범위를 먼저 확인한다.
- 설정값 변경은 가능하면 대응하는 `Config/*.ini`와 C++ 기본값 중 어디가 기준인지 맞춘다.
- 엔진 업그레이드, 플러그인 추가, 입력 체계 변경은 연쇄 영향이 크므로 변경 이유를 분명히 남긴다.

### 응답 원칙
- 변경한 파일과 목적을 짧게 설명한다.
- Unreal 특성상 에디터 확인이 필요한 항목은 별도로 명시한다.
- 테스트하지 못한 항목은 추정으로 표현하지 않는다.

## 8. 코딩 컨벤션 (Epic Games 표준)

### 디렉터리 규칙
- `Source/SpaceRabbit`
  - UE C++ 모듈 루트다.
  - 공개 헤더는 `Public`, 내부 구현은 `Private` 기준으로 분리한다.
- `Source/SpaceRabbit/Public`
  - 다른 모듈이나 리플렉션 경로에서 참조될 수 있는 공개 헤더를 둔다.
  - 클래스 선언, 외부에 노출되는 인터페이스, 필요한 최소한의 타입 선언만 둔다.
- `Source/SpaceRabbit/Private`
  - `.cpp` 파일과 모듈 내부 전용 헤더를 둔다.
  - 외부에 공개할 필요가 없는 구현 세부사항은 `Private`에 둔다.
- `Content`
  - 블루프린트, 맵, 입력 액션, Paper2D 에셋을 둔다.
  - 현재 입력 관련 에셋은 `Content/Input`, 맵은 `Content/Maps`에 있다.
- `Config`
  - 입력, 게임 모드, 맵 기본값 등 프로젝트 설정을 관리한다.
- `Intermediate`, `DerivedDataCache`, `Saved`
  - 생성 산출물로 간주한다. 문제 해결 목적이 아닌 일반 수정 대상이 아니다.

### 명명 규칙
- Epic Games C++ 스타일을 따른다.
- 새 C++ 클래스 추가 시 기본 원칙은 다음과 같다.
  - 헤더 `.h`는 `Public` 또는 `Private` 중 노출 범위에 맞는 폴더에 둔다.
  - 구현 `.cpp`는 `Private`에 둔다.
  - 외부 참조가 필요 없는 클래스는 가능하면 `Private`에 숨긴다.
- 클래스/구조체/열거형 접두사를 유지한다.
  - `A`: Actor
  - `U`: UObject
  - `F`: Struct
  - `E`: Enum
- 불리언은 `b` 접두사를 사용한다.
- 변수와 함수는 PascalCase를 사용한다.
- `UPROPERTY`, `UFUNCTION`에는 목적에 맞는 `Category`를 반드시 지정한다.
- 에디터 노출 범위는 최소화한다.
  - 기본값 조정만 필요하면 `EditDefaultsOnly`
  - 런타임 읽기 중심이면 `BlueprintReadOnly`
- 헤더에는 필요한 선언만 두고, 구현은 `.cpp`에 둔다.
- `#include`는 가능한 한 구체적인 경로로 작성하고, 공개 API는 `Public`, 내부 구현 의존성은 `Private`에 머물게 한다.
- 순환 참조를 줄이기 위해 헤더에서는 전방 선언을 우선하고, 실제 include는 `.cpp`로 내린다.
- Unreal 리플렉션 타입이 필요 없으면 과도한 `UCLASS`, `USTRUCT`, `UENUM` 사용을 피한다.
- 주석은 한글로 짧고 명확하게 작성한다. 복잡한 의도 설명이 필요한 곳만 남긴다.

### 코드 예시
```cpp
// 플레이어 이동 속도 (units/초)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
float MoveSpeed = 300.0f;
 
// 현재 무적 상태 여부
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
bool bIsInvincible = false;
 
// 데미지 처리 함수
UFUNCTION(BlueprintCallable, Category = "Combat")
void TakeDamage(int32 DamageAmount);
```
