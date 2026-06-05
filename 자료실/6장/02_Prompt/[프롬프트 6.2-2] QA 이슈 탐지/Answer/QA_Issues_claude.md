# QA 이슈 리포트 - Claude 분석

**분석 일시**: 2026.02.15
**분석 대상**: SpaceShooter 프로젝트 6.2 버전
**분석 도구**: C++ 정적 코드 분석
**분석 범위**: 초기화 순서, 게임 로직, 리소스 관리

---

## 이슈 #1: 적 총알 수명 타이머 미구현으로 인한 메모리 누적

**우선순위**: Critical (P0)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 에디터에서 GameMap.umap 실행
2. 적 생성 대기 (자동 스폰)
3. 적의 총알 발사 유도 (EncounterRadius 내 진입)
4. 총알이 플레이어나 DestroyZone을 맞추지 못하도록 회피
5. 일정 시간(5초 이상) 경과 후 World Outliner에서 EnemyBulletActor 개수 확인

**예상 동작**:
- 적 총알은 5초(LifeTime) 경과 후 자동으로 비활성화되어야 함
- 월드에 활성 상태로 남아있는 총알 수가 제한되어야 함

**실제 동작**:
- `EnemyBulletActor::SetActive(true)` 호출 시 LifeTimeTimerHandle 설정 코드가 비어있음 (EnemyBulletActor.cpp:82-84)
- 총알이 플레이어를 맞추거나 DestroyZone에 진입하지 않는 한 영구히 활성 상태로 유지
- 게임 진행 중 EnemyBulletActor 인스턴스가 계속 증가하여 메모리 누적 및 성능 저하 발생

**관련 클래스**:
- `AEnemyBulletActor::SetActive()` (EnemyBulletActor.cpp:76-89)
- `AEnemyBulletActor::OnLifeTimeExpired()` (EnemyBulletActor.cpp:103-106)

**로그 확인 항목**:
- [ ] `SetActive(true)` 호출 시 타이머 설정 여부
- [ ] `OnLifeTimeExpired()` 호출 여부 (예상: 호출되지 않음)
- [ ] 게임 시작 후 30초, 1분, 2분 시점의 EnemyBulletActor 개수 추이
- [ ] `stat game` 명령어로 액터 개수 모니터링
- [ ] 메모리 사용량 추이 (Stat Memory)

**실제 로그**: (재현 후 첨부)
```
[PIE 실행 후 Output Log 복사]
[stat game, stat memory 스크린샷]
```

**참조 자료**: (재현 후 첨부)
- World Outliner에서 EnemyBulletActor 개수 증가 스크린샷
- Stat game/memory 캡처

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 성능, 메모리
- 심각도: 장시간 플레이 시 프레임 드롭 및 메모리 부족 가능성
- 제안 사항: SetActive(true) 블록에 타이머 설정 추가

**코드 위치**:
```cpp
// EnemyBulletActor.cpp:82-84 (현재)
if (bActive)
{
    // 빈 블록: LifeTimeTimerHandle 설정 누락
    // 예상 코드:
    // GetWorldTimerManager().SetTimer(LifeTimeTimerHandle, this,
    //     &AEnemyBulletActor::OnLifeTimeExpired, LifeTime, false);
}
```

---

## 이슈 #2: HP 초기화 순서 오류로 인한 게임 시작 시 UI 불일치

**우선순위**: High (P1)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 에디터에서 GameMap.umap 실행
2. PIE(Play In Editor) 시작
3. MainWidget의 HP 아이콘 확인

**예상 동작**:
- 게임 시작 시 플레이어 HP가 MaxHP(3)로 초기화되고, UI에 3개의 HP 아이콘이 표시되어야 함

**실제 동작**:
- `PlayerPawn::BeginPlay()`에서 `CurHP = 0` 상태로 `GameMode->UpdatePlayerHP(0)` 호출 후 `CurHP = MaxHP` 설정
- UI는 0으로 업데이트되었다가 이후 갱신되지 않아 HP 아이콘이 표시되지 않을 가능성 존재

**관련 클래스**:
- `APlayerPawn::BeginPlay()` (PlayerPawn.cpp:66-98)
- `AShootingGameMode::UpdatePlayerHP()` (ShootingGameMode.cpp:60-63)

**로그 확인 항목**:
- [ ] `APlayerPawn::BeginPlay()` 진입 시 CurHP 값 (예상: 0)
- [ ] `GameMode->UpdatePlayerHP()` 호출 시 전달된 인자 (예상: 0)
- [ ] `MainWidget::UpdatePlayerHP()` 호출 후 HP 아이콘 개수
- [ ] `CurHP = MaxHP` 설정 후 CurHP 값 (예상: 3)
- [ ] UI가 다시 업데이트되는지 여부

**실제 로그**: (재현 후 첨부)
```
[PIE 실행 후 Output Log 복사]
```

**참조 자료**: (재현 후 첨부)
- 게임 시작 시 HP UI 스크린샷
- MainWidget의 HP 아이콘 표시 상태

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: UI
- 제안 사항: 77-82행 순서 변경 (CurHP 초기화 후 UI 업데이트)

**코드 위치**:
```cpp
// PlayerPawn.cpp:77-82 (현재)
if (AShootingGameMode* GameMode = Cast<AShootingGameMode>(GetWorld()->GetAuthGameMode()))
{
    GameMode->UpdatePlayerHP(CurHP);  // CurHP = 0 전달
}
CurHP = MaxHP;  // 이후 MaxHP로 설정하지만 UI 재업데이트 없음

// 권장 수정:
CurHP = MaxHP;  // 먼저 초기화
if (AShootingGameMode* GameMode = Cast<AShootingGameMode>(GetWorld()->GetAuthGameMode()))
{
    GameMode->UpdatePlayerHP(CurHP);  // MaxHP 전달
}
```

---

## 이슈 #3: 적 다중 발사 타이머 중복 설정으로 인한 버스트 카운트 오류

**우선순위**: High (P1)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 에디터에서 GameMap.umap 실행
2. 적 생성 대기
3. 적의 EncounterRadius(500) 내에 진입하여 발사 유도
4. 적이 버스트 사격을 완료하기 전에 다시 플레이어에게 접근
5. 출력 로그에서 FireOneBullet 호출 횟수 확인

**예상 동작**:
- 적은 1회 조우당 MinBurstCount ~ MaxBurstCount(3~5발) 범위 내에서만 발사해야 함
- 발사 완료 후 다시 조우할 때만 새로운 버스트 사격 시작

**실제 동작**:
- `EnemyActor::TryStartFiring()`에서 `bHasFired`, `bIsFiring` 가드가 제거됨 (주석 참조: EnemyActor.cpp:91)
- Tick 함수가 매 프레임 `TryStartFiring()` 호출 → EncounterRadius 내에 있는 한 타이머가 반복 설정
- 동일한 적이 TargetBurstCount를 초과하여 발사하거나, 타이머가 중복 누적되어 의도치 않은 발사 패턴 발생

**관련 클래스**:
- `AEnemyActor::Tick()` (EnemyActor.cpp:77-87)
- `AEnemyActor::TryStartFiring()` (EnemyActor.cpp:89-131)
- `AEnemyActor::FireOneBullet()` (EnemyActor.cpp:133-166)

**로그 확인 항목**:
- [ ] `TryStartFiring()` 호출 횟수 (프레임당 1회 * Tick 호출 시)
- [ ] 동일 적의 `FireTimerHandle` 중복 설정 여부
- [ ] `CurrentBurstCount` 값이 `TargetBurstCount`를 초과하는지 확인
- [ ] `FireOneBullet()` 호출 횟수가 의도된 버스트 카운트를 초과하는지 확인
- [ ] `bIsFiring` 플래그 상태 변화 추적

**실제 로그**: (재현 후 첨부)
```
[PIE 실행 후 Output Log 복사]
[UE_LOG로 TryStartFiring, FireOneBullet 호출 로깅 후 첨부]
```

**참조 자료**: (재현 후 첨부)
- 버스트 사격 패턴 영상
- CurrentBurstCount 초과 로그 스크린샷

**추가 정보**:
- 재현 빈도: 항상 (플레이어가 EncounterRadius 내에 머무를 때)
- 영향 범위: 게임플레이, 난이도 밸런스
- 제안 사항: TryStartFiring() 진입 전에 bIsFiring 또는 bHasFired 가드 복원

**코드 위치**:
```cpp
// EnemyActor.cpp:89-131
void AEnemyActor::TryStartFiring()
{
    // 주석: bHasFired / bIsFiring 가드 제거: 중복 타이머 누적 가능

    // 권장 수정: 가드 복원
    // if (bHasFired || bIsFiring) return;
```

---

## 이슈 #4: 적 버스트 카운트 초기화 오류로 1발만 발사

**우선순위**: High (P1)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 에디터에서 GameMap.umap 실행
2. 적 생성 후 EncounterRadius 내 진입
3. 적의 발사 횟수 관찰

**예상 동작**:
- 적은 MinBurstCount ~ MaxBurstCount(3~5발) 사이의 랜덤한 횟수만큼 연속 발사해야 함

**실제 동작**:
- `TryStartFiring()`에서 `CurrentBurstCount = TargetBurstCount`로 초기화 (EnemyActor.cpp:122)
- `FireOneBullet()`에서 `CurrentBurstCount++` 후 `>= TargetBurstCount` 체크 (141, 162행)
- 결과적으로 첫 발 발사 시 `CurrentBurstCount`가 `TargetBurstCount`를 초과하여 즉시 종료
- 의도된 것보다 1발만 발사되거나 발사가 조기 종료됨

**관련 클래스**:
- `AEnemyActor::TryStartFiring()` (EnemyActor.cpp:89-131)
- `AEnemyActor::FireOneBullet()` (EnemyActor.cpp:133-166)

**로그 확인 항목**:
- [ ] `TargetBurstCount` 값 (예상: 3~5)
- [ ] `CurrentBurstCount` 초기값 (현재: TargetBurstCount와 동일)
- [ ] `FireOneBullet()` 호출 후 `CurrentBurstCount` 증가 값
- [ ] 실제 발사된 총알 개수

**실제 로그**: (재현 후 첨부)
```
[PIE 실행 후 Output Log 복사]
```

**참조 자료**: (재현 후 첨부)
- 적 발사 패턴 영상

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 게임플레이
- 제안 사항: `CurrentBurstCount = 0`으로 초기화

**코드 위치**:
```cpp
// EnemyActor.cpp:121-122 (현재)
TargetBurstCount = FMath::RandRange(MinBurstCount, MaxBurstCount);
CurrentBurstCount = TargetBurstCount;  // 오류: 목표값과 동일하게 설정

// 권장 수정:
TargetBurstCount = FMath::RandRange(MinBurstCount, MaxBurstCount);
CurrentBurstCount = 0;  // 0부터 시작
```

---

## 이슈 #5: 플레이어 총알 풀 크기 무제한 증가 가능성

**우선순위**: Medium (P2)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 에디터에서 GameMap.umap 실행
2. 발사 키를 길게 눌러 연속 발사 (10발 이상)
3. 모든 총알이 적을 맞추거나 DestroyZone에 도달하지 않도록 유도
4. World Outliner에서 PlayerBulletActor 개수 확인
5. BulletPool.Num() 값 추적

**예상 동작**:
- 총알 풀은 MaxBulletCount(10)로 제한되어야 함
- 풀이 비어있을 때 새 총알 생성은 허용되나, 풀 크기는 최대치를 유지해야 함

**실제 동작**:
- `FireBullet()`에서 풀이 비어있으면 새 총알을 SpawnActor로 생성 (PlayerPawn.cpp:161-164)
- 생성된 총알이 DestroyZone이나 적과 충돌하여 풀에 반환되면 BulletPool 크기가 MaxBulletCount를 초과할 수 있음
- 장시간 플레이 시 풀 크기가 무한정 증가하여 메모리 사용량 증가 가능

**관련 클래스**:
- `APlayerPawn::FireBullet()` (PlayerPawn.cpp:152-174)
- `APlayerPawn::ReturnBulletToPool()` (PlayerPawn.cpp:257-260)
- `ABulletActor::OnBoxCompOverlap()` (PlayerBulletActor.cpp:75-99)

**로그 확인 항목**:
- [ ] `FireBullet()` 호출 시 BulletPool.Num() 값
- [ ] 새 총알 SpawnActor 호출 횟수
- [ ] `ReturnBulletToPool()` 호출 시 BulletPool.Num() 값
- [ ] BulletPool 크기가 MaxBulletCount(10)를 초과하는지 확인
- [ ] 게임 시작 후 시간 경과에 따른 BulletPool 크기 추이

**실제 로그**: (재현 후 첨부)
```
[PIE 실행 후 Output Log 복사]
[BulletPool.Num() 값 변화 추적 로그]
```

**참조 자료**: (재현 후 첨부)
- World Outliner의 PlayerBulletActor 개수 스크린샷
- BulletPool 크기 증가 추이 그래프

**추가 정보**:
- 재현 빈도: 항상 (장시간 플레이 시)
- 영향 범위: 메모리 관리
- 제안 사항: ReturnBulletToPool()에서 풀 크기 체크 후 초과 시 총알 Destroy

**코드 위치**:
```cpp
// PlayerPawn.cpp:257-260 (현재)
void APlayerPawn::ReturnBulletToPool(ABulletActor* Bullet)
{
    BulletPool.Add(Bullet);  // 크기 제한 없이 무조건 추가
}

// 권장 수정:
void APlayerPawn::ReturnBulletToPool(ABulletActor* Bullet)
{
    if (BulletPool.Num() >= MaxBulletCount)
    {
        Bullet->Destroy();
    }
    else
    {
        BulletPool.Add(Bullet);
    }
}
```

---

## 이슈 #6: EnemyBulletActor 충돌 시 플레이어 사망 처리 누락

**우선순위**: Medium (P2)

**발견 일시**: 2026.02.15

**재현 조건**:
1. 플레이어 HP를 1로 설정 (또는 연속 피격)
2. EnemyBulletActor가 플레이어에 적중
3. HP가 0 이하가 되는 상황 확인

**예상 동작**:
- HP가 0 이하이면 플레이어 파괴/게임 일시정지/GameOver UI 표시가 수행되어야 함

**실제 동작**:
- `AEnemyBulletActor::OnBoxCompOverlap()`에서 `Player->OnDamage(Damage)` 호출만 수행 (EnemyBulletActor.cpp:69)
- `APlayerPawn::OnDamage()`는 HP 감소만 처리하고 사망 로직이 없음 (PlayerPawn.cpp:190-202)
- `AEnemyActor::OnBoxCompOverlap()`에는 사망 처리 로직이 있으나 (EnemyActor.cpp:192-201), EnemyBulletActor에는 없음
- HP 0 이하에서도 게임이 계속 진행됨

**관련 클래스**:
- `AEnemyBulletActor::OnBoxCompOverlap()` (EnemyBulletActor.cpp:59-74)
- `APlayerPawn::OnDamage()` (PlayerPawn.cpp:190-202)
- `AEnemyActor::OnBoxCompOverlap()` (EnemyActor.cpp:178-208) - 참조용

**로그 확인 항목**:
- [ ] `OnDamage()` 호출 시 CurHP 값 변화
- [ ] HP가 0 이하가 되었을 때 추가 처리 여부
- [ ] `GameMode->ShowGameOverUI()` 호출 여부
- [ ] `Player->Destroy()` 호출 여부

**실제 로그**: (재현 후 첨부)
```
[PIE 실행 후 Output Log 복사]
```

**참조 자료**: (재현 후 첨부)
- HP 0 상태에서 게임 진행 스크린샷

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 게임플레이
- 제안 사항: OnDamage() 또는 EnemyBulletActor 충돌 처리에 사망 로직 추가

**코드 위치**:
```cpp
// EnemyBulletActor.cpp:64-73 (현재)
APlayerPawn* Player = Cast<APlayerPawn>(OtherActor);
if (Player)
{
    if (!Player->IsInvincible())
    {
        Player->OnDamage(Damage);  // HP 감소만 수행
        // 사망 처리 누락
    }
    SetActive(false);
}

// 권장 수정: EnemyActor.cpp:192-201 패턴 참조
if (Player->GetHP() <= 0)
{
    Player->Destroy();
    UGameplayStatics::SetGamePaused(GetWorld(), true);
    if (AShootingGameMode* GameMode = Cast<AShootingGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GameMode->ShowGameOverUI();
    }
}
```

---

## 요약

| 이슈 번호 | 우선순위 | 요약 | 영향 범위 |
|----------|---------|------|----------|
| #1 | P0 | 적 총알 수명 타이머 미구현 → 메모리 누적 | 성능, 메모리 |
| #2 | P1 | HP 초기화 순서 오류로 UI 불일치 | UI |
| #3 | P1 | 적 다중 발사 타이머 중복 설정 → 버스트 카운트 오류 | 게임플레이 |
| #4 | P1 | 적 버스트 카운트 초기화 오류 → 1발만 발사 | 게임플레이 |
| #5 | P2 | 플레이어 총알 풀 크기 무제한 증가 | 메모리 |
| #6 | P2 | EnemyBullet 충돌 시 플레이어 사망 처리 누락 | 게임플레이 |

**권장 조치 순서**:
1. **이슈 #1 (P0)**: 적 총알 수명 타이머 즉시 구현 → 메모리 누수 방지
2. **이슈 #4 (P1)**: 버스트 카운트 초기화 수정 → 적 공격 패턴 정상화
3. **이슈 #3 (P1)**: 발사 타이머 가드 복원 → 중복 타이머 방지
4. **이슈 #2 (P1)**: HP 초기화 순서 수정 → 게임 시작 시 올바른 UI 표시
5. **이슈 #6 (P2)**: 사망 처리 로직 추가 → 게임오버 정상 작동
6. **이슈 #5 (P2)**: 풀 크기 제한 추가 → 장기 메모리 안정성 확보

**추가 발견 사항**:
- EnemyActor의 CurHP 초기화 누락 (현재 미사용이지만 미래 확장 시 버그 가능성)
- PlayerPawn의 발사 타이머 중복 설정 가능성 (빠른 연속 입력 시)
- 이러한 이슈들은 기존 QA_Issues.md와 유사하나, 분석 관점과 우선순위가 일부 다름
