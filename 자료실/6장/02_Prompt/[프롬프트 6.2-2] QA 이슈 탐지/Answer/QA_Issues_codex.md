# QA 이슈 리포트

## 이슈 #1: SpawnPoint 미존재 시 EnemyManagerActor 스폰 크래시

**우선순위**: Critical (P0)

**발견 일시**: 2026.02.15 11:10

**재현 조건**:
1. 레벨에 이름/라벨에 "SpawnPoint"가 포함된 Actor를 배치하지 않는다.
2. `EnemyManagerActor`가 존재하는 맵에서 PIE 실행.
3. 스폰 타이머가 첫 호출되는 시점까지 대기.

**예상 동작**:
- 스폰 포인트가 0개면 스폰을 시도하지 않고 안전하게 반환하며 경고 로그를 남긴다.

**실제 동작**:
- `SpawnPoints.Num()`이 0인데 `RandRange(0, -1)` 및 `% SpawnPoints.Num()` 경로로 진입하여 크래시/ensure 발생.

**관련 클래스**:
- `AEnemyManagerActor::BeginPlay`
- `AEnemyManagerActor::SpawnEnemy`
- `Source/SpaceShooter/Private/EnemyManagerActor.cpp`

**로그 확인 항목**:
- [ ] `AEnemyManagerActor::BeginPlay`에서 `SpawnPoints.Num()` 값
- [ ] `AEnemyManagerActor::SpawnEnemy` 진입 시 `SpawnPoints.Num()`/`SpawnIndex` 값
- [ ] `SpawnTimerHandle` 설정 성공 여부 및 호출 주기

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 게임플레이, 크래시

---

## 이슈 #2: BulletFactory 미지정 시 PlayerPawn 풀 생성 중 널 포인터 접근

**우선순위**: High (P1)

**발견 일시**: 2026.02.15 11:10

**재현 조건**:
1. `PlayerPawn`의 `BulletFactory`를 비워둔다.
2. PIE 실행.
3. `APlayerPawn::BeginPlay`에서 풀 초기화가 수행됨.

**예상 동작**:
- Bullet 클래스가 없으면 풀 생성 없이 경고 로그를 출력하고 안전하게 진행한다.

**실제 동작**:
- `SpawnActor` 실패로 `Bullet`가 null인데 `Bullet->SetActive(false)`를 호출하여 크래시 가능.

**관련 클래스**:
- `APlayerPawn::BeginPlay`
- `Source/SpaceShooter/Private/PlayerPawn.cpp`

**로그 확인 항목**:
- [ ] `BulletFactory` 에셋 참조 유무
- [ ] 풀 초기화 루프에서 `SpawnActor` 반환값
- [ ] `MaxBulletCount` 값 및 풀 길이

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 게임플레이, 크래시, 리소스 관리(풀)

---

## 이슈 #3: 적 탄환 피격으로 HP 0 이하가 되어도 GameOver 처리 누락

**우선순위**: High (P1)

**발견 일시**: 2026.02.15 11:10

**재현 조건**:
1. 플레이어 HP를 1로 설정 (또는 연속 피격).
2. `EnemyBulletActor`가 플레이어에 적중.
3. HP가 0 이하가 되는 상황 확인.

**예상 동작**:
- HP가 0 이하이면 플레이어 파괴/게임 일시정지/GameOver UI 표시가 수행된다.

**실제 동작**:
- `APlayerPawn::OnDamage`는 HP만 감소시키고 사망 처리 로직이 없어 게임이 계속 진행됨.

**관련 클래스**:
- `AEnemyBulletActor::OnBoxCompOverlap`
- `APlayerPawn::OnDamage`
- `Source/SpaceShooter/Private/EnemyBulletActor.cpp`
- `Source/SpaceShooter/Private/PlayerPawn.cpp`

**로그 확인 항목**:
- [ ] `APlayerPawn::OnDamage` 호출 시 `CurHP` 변화
- [ ] `GameMode->ShowGameOverUI()` 호출 여부
- [ ] 플레이어 파괴 시점 (`PlayerPawn::Destroy`) 유무

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 게임플레이

---

## 이슈 #4: 적 연사(Burst) 카운트 초기화 오류로 1발만 발사

**우선순위**: Medium (P2)

**발견 일시**: 2026.02.15 11:10

**재현 조건**:
1. 플레이어가 적의 `EncounterRadius` 내로 진입.
2. 적이 연사 시작.
3. 발사 횟수 확인.

**예상 동작**:
- `MinBurstCount~MaxBurstCount` 범위만큼 연속 발사 후 종료.

**실제 동작**:
- `CurrentBurstCount = TargetBurstCount`로 초기화되어 첫 발 후 즉시 종료되는 경우 발생.

**관련 클래스**:
- `AEnemyActor::TryStartFiring`
- `AEnemyActor::FireOneBullet`
- `Source/SpaceShooter/Private/EnemyActor.cpp`

**로그 확인 항목**:
- [ ] `TargetBurstCount`/`CurrentBurstCount` 초기값
- [ ] `FireOneBullet` 호출 횟수
- [ ] `FireTimerHandle` 설정/해제 타이밍

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 게임플레이

---

## 이슈 #5: EnemyBulletActor 수명 타이머 미설정으로 탄환 무한 유지

**우선순위**: Medium (P2)

**발견 일시**: 2026.02.15 11:10

**재현 조건**:
1. 적이 탄환 발사.
2. 탄환이 파괴/비활성화되지 않는 경로(플레이어 미적중, DestroyZone 미진입) 확인.
3. `LifeTime` 경과 후 상태 확인.

**예상 동작**:
- `LifeTime` 경과 시 `OnLifeTimeExpired`가 호출되어 탄환이 비활성화된다.

**실제 동작**:
- `SetActive(true)` 시 `LifeTimeTimerHandle`가 설정되지 않아 탄환이 계속 유지됨.

**관련 클래스**:
- `AEnemyBulletActor::SetActive`
- `AEnemyBulletActor::OnLifeTimeExpired`
- `Source/SpaceShooter/Private/EnemyBulletActor.cpp`

**로그 확인 항목**:
- [ ] `SetActive(true)` 호출 시 `LifeTimeTimerHandle` 설정 여부
- [ ] `LifeTime` 값
- [ ] 장시간 플레이 시 액터 수 증가 여부

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 성능, 리소스 관리

---

## 이슈 #6: 초기 HP UI 갱신 순서 오류로 시작 시 HP 표시가 0

**우선순위**: Low (P3)

**발견 일시**: 2026.02.15 11:10

**재현 조건**:
1. 게임 시작 시 MainWidget HP 표시 확인.
2. 추가 피해 없이 초기 표시 상태 관찰.

**예상 동작**:
- 시작 직후 UI에 `MaxHP`가 정상 표시된다.

**실제 동작**:
- `APlayerPawn::BeginPlay`에서 `CurHP` 설정 이전에 `UpdatePlayerHP`가 호출되어 0으로 표시됨.

**관련 클래스**:
- `APlayerPawn::BeginPlay`
- `AShootingGameMode::UpdatePlayerHP`
- `Source/SpaceShooter/Private/PlayerPawn.cpp`
- `Source/SpaceShooter/Private/ShootingGameMode.cpp`

**로그 확인 항목**:
- [ ] `CurHP` 초기값 및 `MaxHP` 값
- [ ] `UpdatePlayerHP` 호출 시점
- [ ] UI 위젯의 HP 표시 값

**실제 로그**: (재현 후 첨부)
```
```

**참조 자료**: (재현 후 첨부)

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: UI
