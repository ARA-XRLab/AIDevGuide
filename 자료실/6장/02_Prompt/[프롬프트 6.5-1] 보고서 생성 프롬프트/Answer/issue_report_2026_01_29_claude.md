# QA Issue Report (2026-01-29)

## 요약
- 수정된 이슈: 3개
- 남은 이슈: 2개
- 판단 요약: UI 초기화, 게임플레이 로직 버그 해결 완료, 방어 코드 및 성능 이슈 추가 검증 필요

## 수정 내역

### Issue #1: 시작 시 플레이어 HP UI가 0으로 표시됨
- 우선순위: P2
- 수정 내용: BeginPlay 시 HP 초기화 및 UI 업데이트 로직 수정
- 검증 방법: PIE 실행 후 HP UI 정상 표시 확인

### Issue #2: 적 발사 패턴이 종료되지 않고 반복 실행되는 문제
- 우선순위: P2
- 수정 내용: CurrentBurstCount 초기화 값을 0으로 수정, TryStartFiring에서 bHasFired 검사 추가
- 검증 방법: PIE 실행 후 발사 패턴 정상 표시 확인

### Issue #4: 적 총알 수명이 적용되지 않아 장시간 잔존
- 우선순위: P3
- 수정 내용: SetActive(true) 시점에 LifeTime 타이머 시작, OnLifeTimeExpired()에서 Destroy() 호출 추가
- 검증 방법: ActorTracking.log로 BP_EnemyBullet_C 개수 안정화 확인 (12~21 범위 유지)

## 남은 이슈

### Issue #3: SpawnPoint가 없으면 적 스폰 시 크래시 가능
- 우선순위: P1
- 관리 대상 사유: 방어 코드 추가 필요, 실제 크래시 재현 및 검증 미완료

### Issue #5: SpawnFrequency 비정상 설정으로 성능 저하
- 우선순위: P1
- 관리 대상 사유: 2026-01-30 발견, 해결 방법 제시되었으나 검증 미완료

## 참고 자료
- 관련 로그 파일: ActorTracking.log
- 트레이스 파일: Unreal Insights 트레이스, TimerStats.csv
