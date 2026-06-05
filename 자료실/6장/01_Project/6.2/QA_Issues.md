# QA 이슈 리포트

## 이슈 #1: 시작 시 플레이어 HP UI가 0으로 표시됨

**우선순위**: Medium (P2)

**발견 일시**: 2026.01.29 11:00

**재현 조건**:
1. 에디터에서 레벨을 열고 PIE 실행
2. 게임 시작 직후 HP UI 확인
3. 아무 행동 없이 표시 상태 확인

**예상 동작**:
- 시작 시 플레이어 HP(기본 3)가 UI에 표시되어야 함

**실제 동작**:
- 시작 직후 HP 아이콘이 모두 숨김 상태로 표시됨

**관련 클래스**:
- APlayerPawn::BeginPlay
- AShootingGameMode::UpdatePlayerHP
- UMainWidget::UpdatePlayerHP

**로그 확인 항목**:
- [ ] BeginPlay 시 CurHP 값
- [ ] UpdatePlayerHP 호출 시점
- [ ] UMainWidget 바인딩 유효성

**실제 로그**: (재현 시 첨부)
- 해당 이슈는 UI 출력으로 즉시 검증 가능하여 로그 수집을 생략함

**참조 자료**: (재현 시 첨부)
- 없음

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: UI

**상태**: 해결 완료 (2026.01.29)
**검증 방법**: PIE 실행 후 HP UI 정상 표시 확인


---

## 이슈 #2: 적 발사 패턴이 종료되지 않고 반복 실행되는 문제

**우선순위**: Medium (P2)

**발견 일시**: 2026.01.29 11:00

**재현 조건**:
1. 플레이어와 적이 가까운 상태에서 게임 시작
2. 적이 사거리(EncounterRadius) 안에 들어오도록 유도
3. 적의 발사 패턴을 일정 시간 관찰

**예상 동작**:
- 적은 MinBurstCount~MaxBurstCount 범위로 연사한 뒤
  발사를 종료하고 다음 상태(이동/대기)로 전환되어야 함

**실제 동작**:
- 적의 발사가 종료되지 않고 발사 시작 로직이 반복적으로 재호출됨
- 결과적으로 연사 패턴이 계속 유지되거나 즉시 재시작됨

**관련 클래스**:
- AEnemyActor::Tick
- AEnemyActor::TryStartFiring
- AEnemyActor::FireOneBullet
- AEnemyActor::EndFiring

**로그 확인 항목**:
- [ ] TargetBurstCount 값
- [ ] CurrentBurstCount 증가/초기화 시점
- [ ] EndFiring 호출 시점
- [ ] 타이머(FireTimerHandle) 설정/해제 여부

**실제 로그**: (재현 시 첨부)
```
N/A
```

**참조 자료**: (재현 시 첨부)
- 없음

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 게임플레이

**해결 방법**:
1. `CurrentBurstCount` 초기화 값을 0으로 수정
2. `TryStartFiring`에서 `bHasFired` 검사 추가

**상태**: 해결 완료 (2026.01.29)
**검증 방법**: PIE 실행 후 발사 패턴 정상 표시 확인

---

## 이슈 #3: SpawnPoint가 없으면 적 스폰 시 크래시 가능

**우선순위**: High (P1)

**발견 일시**: 2026.01.29 11:00

**재현 조건**:
1. 레벨에서 "SpawnPoint" 이름이 포함된 액터를 제거
2. 게임 시작 후 적 스폰 타이머 대기
3. 적 스폰 시점 관찰

**예상 동작**:
- 스폰 지점이 없으면 스폰을 건너뛰고 경고 로그 출력

**실제 동작**:
- SpawnPoints가 비어 있을 때 인덱스 계산이 실패하여 크래시 가능

**관련 클래스**:
- AEnemyManagerActor::BeginPlay
- AEnemyManagerActor::SpawnEnemy

**로그 확인 항목**:
- [ ] SpawnPoints.Num() 값
- [ ] SpawnIndex 계산 결과
- [ ] 스폰 타이머 상태

**실제 로그**: (재현 시 첨부)
```
N/A
```

**참조 자료**: (재현 시 첨부)
- 없음

**추가 정보**:
- 재현 빈도: 특정 조건(SpawnPoint 없음)
- 영향 범위: 게임플레이

---

## 이슈 #4: 적 총알 수명이 적용되지 않아 장시간 잔존

**우선순위**: Low (P3)

**발견 일시**: 2026.01.29 11:00

**재현 조건**:
1. 적이 발사한 총알을 피하면서 일정 시간 유지
2. LifeTime(기본 5초) 이후에도 총알이 남아있는지 확인

**예상 동작**:
- LifeTime이 지나면 총알이 자동 비활성화/제거되어야 함

**실제 동작**:
- LifeTime 타이머가 설정되지 않아 총알이 계속 유지됨

**관련 클래스**:
- AEnemyBulletActor::SetActive
- AEnemyBulletActor::OnLifeTimeExpired

**로그 확인 항목**:
- [ ] LifeTime 값
- [ ] LifeTime 타이머 설정 여부
- [ ] OnLifeTimeExpired 호출 여부

**실제 로그**: (재현 시 첨부)
```
N/A
```

**참조 자료**: (재현 시 첨부)
- 없음

**추가 정보**:
- 재현 빈도: 항상
- 영향 범위: 게임플레이