# AGENTS.md

이 문서는 이 저장소에서 작업하는 AI 코딩 에이전트를 위한 지침입니다. 모든 응답과 작업 기록은 특별한 요청이 없으면 한국어로 작성합니다.

## 프로젝트 개요

- Unreal Engine 5.7 C++ 프로젝트입니다.
- 프로젝트 파일: `TPSProject.uproject`
- 주 모듈: `TPSProject` (`Source/TPSProject`)
- 타깃:
  - 게임: `TPSProjectTarget`
  - 에디터: `TPSProjectEditorTarget`
- 현재 주요 의존성: `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`
- 기본 렌더링 설정은 DX12, SM6, Lumen/Nanite 계열 고사양 데스크톱 구성을 기준으로 합니다.

## 작업 원칙

- 변경 전에는 관련 파일을 먼저 읽고, 기존 Unreal 프로젝트 구조와 스타일을 따른다.
- 사용자가 명시하지 않은 대규모 리팩터링, 폴더 재배치, 에셋 이름 변경은 하지 않는다.
- `Binaries/`, `DerivedDataCache/`, `Intermediate/`, `Saved/`, `.vs/`, `.idea/`는 생성 산출물로 취급한다. 직접 수정하거나 핵심 변경 파일로 삼지 않는다.
- `.uproject`, `Config/*.ini`, `Source/**/*.Build.cs`, `Source/**/*.Target.cs` 변경은 빌드와 에디터 로딩에 직접 영향을 주므로 최소 범위로 수정한다.
- Blueprint 또는 에셋 변경이 필요한 경우, 텍스트 파일만으로 안전하게 처리할 수 없으면 사용자에게 에디터 작업이 필요하다고 명확히 알린다.
- 한국어 설명을 우선하되, Unreal API 이름, 클래스명, 함수명, 로그 키워드는 원문을 유지한다.

## C++ 스타일

- Unreal C++ 관례를 따른다.
  - 클래스 접두사: `A`, `U`, `F`, `I`, `E`, `T`
  - 멤버 변수와 함수는 Unreal 명명 규칙을 따른다.
  - UObject 파생 타입은 필요한 경우 `UCLASS`, `USTRUCT`, `UENUM`, `UPROPERTY`, `UFUNCTION` 매크로를 정확히 사용한다.
- include는 필요한 헤더만 추가하고, 가능하면 전방 선언을 사용한다.
- `Tick`은 실제로 매 프레임 처리가 필요할 때만 활성화한다.
- `ConstructorHelpers` 경로 하드코딩은 꼭 필요한 경우에만 사용하고, 에셋 참조는 가능하면 에디터에서 지정 가능한 `UPROPERTY`로 노출한다.
- 로그는 새 카테고리가 필요한 경우 명확한 이름으로 선언하고, 임시 디버그 로그는 작업 완료 전 제거하거나 의도적으로 남긴 이유를 설명한다.
- 네트워크, 입력, 물리, 월드 파티션, 렌더링 설정 변경은 영향 범위를 설명하고 관련 설정 파일을 함께 확인한다.

## 빌드 및 실행

Windows에서 로컬 UE 설치 경로가 기본 설치(`C:\Program Files\Epic Games\UE_5.7`)라면 다음 명령으로 에디터 타깃을 빌드한다.

```powershell
& "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" TPSProjectEditor Win64 Development "C:\Work\Book\TPSProject\TPSProject.uproject" -WaitMutex -NoHotReloadFromIDE
```

에디터 실행 확인이 필요하면 다음 형식을 사용한다.

```powershell
& "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Work\Book\TPSProject\TPSProject.uproject"
```

자동화 테스트가 추가된 경우에는 가능한 범위에서 `UnrealEditor-Cmd.exe`와 `-ExecCmds="Automation RunTests ..."` 형식으로 실행한다. 테스트가 없으면 빌드 성공과 에디터 로딩 로그 확인을 최소 검증으로 삼는다.

## 설정 파일 지침

- `Config/DefaultEngine.ini`는 렌더링, 플랫폼 RHI, 맵, 리다이렉트 설정을 포함한다. 중복 키나 `+`/`-` 배열 연산자의 의미를 확인하고 수정한다.
- `Config/DefaultInput.ini`는 Enhanced Input 사용 설정을 포함한다. 기존 입력 시스템과 Enhanced Input 설정을 혼용할 때는 의도와 호환성을 확인한다.
- 새 모듈이나 플러그인을 추가하면 `*.Build.cs`, `*.Target.cs`, `TPSProject.uproject`의 변경이 서로 맞는지 확인한다.

## 검증 체크리스트

- C++ 또는 빌드 설정 변경 후 `TPSProjectEditor Win64 Development` 빌드를 실행한다.
- `.ini` 변경 후 에디터 시작 로그에서 오류, ensure, missing class, redirect 실패가 없는지 확인한다.
- 새 UObject/Actor/Component를 추가한 경우:
  - 헤더에 생성 코드 include(`*.generated.h`) 위치가 올바른지 확인한다.
  - 모듈 의존성이 부족하지 않은지 확인한다.
  - Blueprint 노출이 필요한 속성은 카테고리와 접근 권한을 명확히 지정한다.
- 사용자 변경 사항이나 생성된 에셋을 임의로 되돌리지 않는다.

## 응답 방식

- 작업 결과는 변경 파일, 검증 명령, 남은 위험을 짧게 정리한다.
- 빌드를 실행하지 못했으면 이유를 명확히 적고, 대신 수행한 정적 검증을 말한다.
- Unreal Editor에서 직접 확인해야 하는 내용은 에디터 확인 항목으로 분리해서 안내한다.

## 프로젝트 전용 개발 지침

### 프로젝트 철학

- 기능 구현-동작 확인-프롬프트 개선-반복 흐름을 따른다.

### 개발 단계

- 프로토타입: 핵심 기능을 우선 구현하고, 단순한 구조로 빠르게 실험한다.
- 알파: 구조를 안정화하고, 필요한 리팩터링을 수행하며, 시스템 책임을 분리한다.
- 베타: 성능 최적화와 사용성 폴리싱을 중심으로 마무리한다.

### 협업 원칙

- 초급~중급 개발자도 따라가기 쉽게 설명하고 작성한다.
- 클래스, 시스템, 파일 단위의 책임 분리를 명확히 한다.
- 같은 요청을 프롬프트로 다시 실행해도 재현 가능한 구조를 지향한다.

### 코딩 규칙

- 클래스, 변수, 함수마다 한글 주석을 작성한다.
- `Tick`은 기본 비활성화하고, 실제로 필요할 때만 활성화한다.
- 디버그 로그는 기본 노란색으로 표시하고, 오류 또는 위험 상황은 빨간색으로 표시한다.
- 에디터에서 조정할 값은 `Category`를 지정해 노출한다.

### 소스 구조

- 헤더 파일은 `Public`에 둔다.
- 구현 파일은 `Private`에 둔다.

### 에디터 연동

- 블루프린트 애셋은 에디터에서 직접 생성한다.
- 애셋 경로를 코드에 직접 적지 않는다.
- 조정 가능한 수치는 에디터에서 변경할 수 있게 노출한다.

### 응답 형식

- 새 파일을 만들 때는 전체 코드를 출력한다.
- 기존 파일을 수정할 때는 바뀐 부분만 출력한다.
