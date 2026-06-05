# QA Issue Report (2026-01-29)

## 요약
- 수정된 이슈: 3개
- 남은 이슈: 2개
- 판단 요약: 플레이어 UI 초기화 및 적의 비정상적인 발사/수명 로직을 수정하여 게임플레이의 안정성을 확보함.

## 수정 내역

### Issue #1 시작 시 플레이어 HP UI가 0으로 표시됨
- 우선순위: Medium (P2)
- 수정 내용: `BeginPlay` 시점에 플레이어 HP 정보를 UI에 강제로 업데이트하여 초기 표시 오류 수정.
- 검증 방법: PIE 실행 후 HP UI 정상 표시 확인

### Issue #2 적 발사 패턴이 종료되지 않고 반복 실행되는 문제
- 우선순위: Medium (P2)
- 수정 내용: `CurrentBurstCount` 초기화 값을 0으로 수정하고, `TryStartFiring`에서 `bHasFired` 상태 검사를 추가하여 중복 발사 방지.
- 검증 방법: PIE 실행 후 적의 발사 패턴(연사 후 중지) 정상 동작 확인

### Issue #4 적 총알 수명이 적용되지 않아 장시간 잔존
- 우선순위: Low (P3)
- 수정 내용: `SetActive(true)` 시점에 `LifeTime` 타이머를 시작하고, 만료 시 `Destroy()`가 호출되도록 로직 보완.
- 검증 방법: ActorTracking.log로 `BP_EnemyBullet_C` 개수가 일정 범위(12~21) 내에서 안정화됨을 확인

## 남은 이슈

### Issue #3 SpawnPoint가 없으면 적 스폰 시 크래시 가능
- 관리 대상 사유: 현재 레벨에 SpawnPoint가 존재하여 즉각적인 크래시는 발생하지 않으나, 예외 처리를 위한 코드 보완 필요 (Open 상태).

### Issue #5 SpawnFrequency 비정상 설정으로 성능 저하
- 관리 대상 사유: 2026-01-30에 발견된 이슈로, 차기 수정 및 검증 대상에 포함됨.

## 참고 자료
- 관련 로그 파일: ActorTracking.log
- 트레이스 파일: Unreal Insights 트레이스 데이터 (Issue #5 관련)
