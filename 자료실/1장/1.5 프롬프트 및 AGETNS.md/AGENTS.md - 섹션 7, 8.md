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
