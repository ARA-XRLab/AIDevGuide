# QA Issue Report (2026-01-29)

## 요약
- 수정된 이슈 3건
- 남은 이슈 2건
- 판단 요약 초기화/타이머/상태 관리 누락을 보완해 정상 동작으로 복구된 이슈가 대부분

## 수정 내역

### Issue #1 시작 시 플레이어 HP UI가 0으로 표시됨
- 우선순위 P2
- 수정 내용 플레이어 HP 초기화 후 UI 갱신 순서 정리 및 표시 로직 보정
- 검증 방법 PIE 실행 후 HP UI 정상 표시 확인

### Issue #2 적 발사 패턴이 종료되지 않고 반복 실행되는 문제
- 우선순위 P2
- 수정 내용 CurrentBurstCount 초기화(0) 및 TryStartFiring에서 bHasFired 검사 추가
- 검증 방법 PIE 실행 후 발사 패턴 정상 표시 확인

### Issue #4 적 총알 수명이 적용되지 않아 장시간 잔존
- 우선순위 P3
- 수정 내용 SetActive(true) 시 LifeTime 타이머 시작, OnLifeTimeExpired에서 Destroy 호출
- 검증 방법 ActorTracking.log로 BP_EnemyBullet_C 개수 안정화 확인 (12~21 범위)

## 남은 이슈

### Issue #3 SpawnPoint가 없으면 적 스폰 시 크래시 가능
- 관리 대상 사유 상태 미기재(Open)로 보이며 SpawnPoint 미존재 시 방어 로직 미적용

### Issue #5 SpawnFrequency 비정상 설정으로 성능 저하
- 관리 대상 사유 해결 방법 제안만 존재하고 상태 미기재(Open)로 남아 있음 (2026-01-30 발견)

## 참고 자료
- 관련 로그 파일 ActorTracking.log
- 트레이스 파일 Unreal Insights 트레이스, TimerStats.csv