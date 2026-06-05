# QA_Issues_GEMINI.md

## 이슈 #01: PlayerPawn 초기 HP UI 표시 오류

**우선순위**: Critical (P0)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 게임 시작 (PIE 실행)
2. 화면 상단의 Player HP UI 확인

**예상 동작**:
- 게임 시작 시 HP가 `MaxHP` 값으로 초기화되어 UI에 표시되어야 함.

**실제 동작**:
- `BeginPlay()`에서 `UpdatePlayerHP()`를 호출할 때 `CurHP`가 초기화되기 전 값이 전달됨.
- 이로 인해 초기 UI 상의 HP가 0 또는 의도하지 않은 값으로 보일 수 있음.

**관련 클래스**:
- `APlayerPawn::BeginPlay()`

**로그 확인 항목**:
- [ ] `APlayerPawn::BeginPlay()` 내 `UpdatePlayerHP` 호출 시 `CurHP` 변수 값
- [ ] `AShootingGameMode::UpdatePlayerHP`에 전달되는 인자 값

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)
- `Source/SpaceShooter/Private/PlayerPawn.cpp:81-86`

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: UI / UX

---

## 이슈 #02: BulletFactory 미할당 시 엔진 크래시 위험

**우선순위**: Critical (P0)

**발견 일시**: 2026.02.15

**재현 조건**:
1. `BP_PlayerPawn` 블루프린트에서 `BulletFactory` 변수를 None으로 설정
2. 게임 실행

**예상 동작**:
- `BulletFactory`가 유효하지 않을 경우 경고 로그를 출력하고 실행을 중단하거나 건너뛰어야 함.

**실제 동작**:
- `SpawnActor<ABulletActor>(BulletFactory)` 호출 시 Null Class를 인자로 전달하여 엔진 크래시 발생 가능성이 높음.

**관련 클래스**:
- `APlayerPawn::BeginPlay()`

**로그 확인 항목**:
- [ ] `SpawnActor` 호출 직전 `BulletFactory`의 유효성 검사 여부
- [ ] `BulletPool`에 추가된 Bullet 객체의 유효성

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)
- `Source/SpaceShooter/Private/PlayerPawn.cpp:95`

**추가 정보**:
- 재현 빈도: 항상 (데이터 미설정 시)
- 영향 범위: 시스템 안정성 (크래시)

---

## 이슈 #03: 플레이어 파괴 시 Null 참조 및 Dangling Pointer 위험

**우선순위**: High (P1)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 적(Enemy)과 충돌하여 플레이어 HP가 0 이하가 되도록 함.
2. 플레이어가 파괴된 이후 다른 시스템(EnemyManager 등)이 플레이어 위치를 참조하려고 할 때 확인.

**예상 동작**:
- 플레이어 파괴 시 안전하게 게임 종료 처리를 하고, 다른 객체들이 플레이어 포인터를 참조하지 않도록 해야 함.

**실제 동작**:
- `Player->Destroy()`를 직접 호출하여 액터를 제거함.
- `EnemyActor` 등에서 `GetPawn()`을 통해 플레이어를 찾으려 할 때 이미 파괴된 객체를 참조하여 런타임 에러 발생 가능.

**관련 클래스**:
- `AEnemyActor::OnBoxCompOverlap()`

**로그 확인 항목**:
- [ ] `Player->Destroy()` 호출 이후 `GetFirstPlayerController()->GetPawn()`의 반환 값
- [ ] 타 클래스에서 Player 포인터 참조 시 Null Check 로직 존재 여부

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)
- `Source/SpaceShooter/Private/EnemyActor.cpp:177`

**추가 정보**:
- 재현 빈도: 플레이어 사망 시 항상
- 영향 범위: 게임플레이 / 안정성

---

## 이슈 #04: 적 총알(EnemyBullet) 수명 타이머 누락

**우선순위**: High (P1)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 적이 발사한 총알이 화면 밖으로 나갔으나 `DestroyZone`에 닿지 않는 상황 연출.
2. 시간이 지난 후에도 메모리에 해당 객체가 남아 있는지 확인.

**예상 동작**:
- 발사된 총알은 일정 시간(LifeTime)이 지나면 자동으로 비활성화되거나 파괴되어야 함.

**실제 동작**:
- `AEnemyBulletActor::SetActive(true)` 호출 시 타이머를 시작하는 구현이 누락되어 있음.
- 화면 밖으로 나간 객체가 해제되지 않아 메모리 누적 발생 가능.

**관련 클래스**:
- `AEnemyBulletActor::SetActive()`

**로그 확인 항목**:
- [ ] `SetActive(true)` 시 `SetTimer` 호출 여부
- [ ] `LifeTimeTimerHandle`의 활성화 상태

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)
- `Source/SpaceShooter/Private/EnemyBulletActor.cpp:84`

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 자원 관리 / 성능

---

## 이슈 #05: SpawnPoint 검색 로직의 성능 저하

**우선순위**: Medium (P2)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 수천 개의 액터가 배치된 대규모 맵 로드.
2. `EnemyManagerActor`가 포함된 레벨 진입 시 로딩 시간 측정.

**예상 동작**:
- 스폰 포인트를 효율적으로 찾아 초기화 시간이 최소화되어야 함.

**실제 동작**:
- `TActorIterator<AActor>`를 사용하여 월드의 모든 액터를 순회하며 이름 문자열 비교(`Contains`)를 수행함.
- 씬의 복잡도에 따라 `BeginPlay` 시점에 심각한 성능 부하 유발 가능.

**관련 클래스**:
- `AEnemyManagerActor::BeginPlay()`

**로그 확인 항목**:
- [ ] `BeginPlay` 함수의 실행 소요 시간 (ms)
- [ ] `SpawnPoints` 배열에 담긴 객체의 타입 및 개수

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)
- `Source/SpaceShooter/Private/EnemyManagerActor.cpp:25-33`

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 성능 / 로딩 속도

---

## 이슈 #06: 적 연사(Burst Fire) 횟수 카운팅 로직 오류

**우선순위**: Medium (P2)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 적이 사거리 내에 들어와 연사를 시작할 때 실제 발사되는 총알 개수 확인.

**예상 동작**:
- `TargetBurstCount`로 설정된 값만큼 정확히 발사되어야 함.

**실제 동작**:
- `FireOneBullet()` 시작 시 `CurrentBurstCount++`를 수행하고 즉시 `>= TargetBurstCount`를 체크함.
- 초기값이 0일 경우, 설정된 값보다 1발 적게 발사되거나 종료 조건이 불명확할 수 있음.

**관련 클래스**:
- `AEnemyActor::FireOneBullet()`
- `AEnemyActor::TryStartFiring()`

**로그 확인 항목**:
- [ ] `TargetBurstCount` 랜덤 생성 값
- [ ] `FireOneBullet` 호출 시 `CurrentBurstCount` 변화 추이

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)
- `Source/SpaceShooter/Private/EnemyActor.cpp:131, 150`

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 게임플레이 밸런스
