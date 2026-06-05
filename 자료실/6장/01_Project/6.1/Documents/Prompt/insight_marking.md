[목표]
현재 프로젝트 코드에 Unreal Insights 분석용 마킹을 자동 삽입하라.

[금지]
- 모든 함수에 마킹 금지
- 단순 함수 / Getter / Setter 마킹 금지
- UI 단순 바인딩 함수 마킹 금지
- 데이터 구조체 마킹 금지

[탐지 규칙]
- Tick / Update 루프
- Spawn / Destroy / Pool miss 경로
- Collision / Trace 반복
- 동기화 대기(Wait, Flush, Fence, TaskGraph)
- UI 갱신 루프(SetText, Invalidate, Rebuild)
- 동적 로드(AssetManager, Streamable, LoadObject)
- 컨테이너 대량 반복(TArray/TMap loop)

[삽입 규칙]
- CPU Scope: TRACE_CPUPROFILER_EVENT_SCOPE
- Bookmark: TRACE_BOOKMARK
- Scope ≤ 15
- Bookmark ≤ 5
- 함수 전체 마킹 금지 → 의심 블록 최소 범위 삽입

[네이밍]
- Scope: SS_<System>_<Action>
- Bookmark: BM_<System>_<Phase>

[출력]
- 수정 파일 목록
- diff
- 삽입 위치(파일:라인)
- 생성된 Scope/Bookmark 리스트