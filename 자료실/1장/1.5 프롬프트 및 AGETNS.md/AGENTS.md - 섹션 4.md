## 4. 게임 오브젝트

### 플레이어 (Space Rabbit)
- 클래스명: ASpaceRabbitCharacter
- 부모 클래스: APawn
- 역할: 플레이어가 조작하는 주인공 캐릭터
컴포넌트:
- UCapsuleComponent: 물리 충돌 처리
- UPaperFlipbookComponent: 토끼 애니메이션

주요 변수:
- float JumpForce = 400.0f (점프력)
- bool bIsJumping = false (점프 중 여부)
- bool bIsGrounded = true (바닥 접촉 여부)
- int32 JumpCount = 0 (현재 점프 횟수, 최대 2)
동작:
1. 자동으로 오른쪽 방향 이동 (실제로는 배경이 왼쪽으로)
2. 스페이스바 입력 시 점프 (2단 점프 가능)
3. 장애물/로켓 충돌 시 게임오버 이벤트 발생
4. 바닥 추락 시 게임오버 이벤트 발생

---
### 코인 (Coin)
- 클래스명: ACoin
- 부모 클래스: AActor
- 역할: 플레이어가 수집하는 점수 아이템
컴포넌트:
- UPaperSpriteComponent: 코인 스프라이트
- USphereComponent: 충돌 감지 (Overlap)
주요 변수:
- int32 CoinValue = 1
동작:
1. 바닥 발판과 함께 왼쪽으로 이동
2. 플레이어와 Overlap 시 점수 추가 후 Destroy

---
### 장애물 (Obstacle)
- 클래스명: AObstacle
- 부모 클래스: AActor
- 역할: 충돌 시 즉시 게임오버를 유발하는 위협 요소
컴포넌트:
- UPaperSpriteComponent: 바위 스프라이트
- UBoxComponent: 충돌 감지 (Overlap)
동작:
1. 바닥 발판 위에 스폰
2. 발판과 함께 왼쪽으로 이동
3. 플레이어와 충돌 시 게임오버

---
### 로켓 (Rocket)
- 클래스명: ARocket
- 부모 클래스: AActor
- 역할: 상공에서 날아오는 위협 요소
컴포넌트:
- UPaperSpriteComponent: 로켓 스프라이트
- UBoxComponent: 충돌 감지
주요 변수:
- float RocketSpeed = 500.0f
동작:
1. 화면 오른쪽 상단에서 스폰
2. 왼쪽으로 직선 이동
3. 플레이어와 충돌 시 게임오버

---
### 바닥 발판 (FloorTile)
- 클래스명: AFloorTile
- 부모 클래스: AActor
- 역할: 플레이어가 딛고 달리는 바닥
컴포넌트:
- UPaperSpriteComponent: 발판 스프라이트
- UBoxComponent: 물리 충돌
주요 변수:
- float TileWidth = 500.0f
- float MoveSpeed = 300.0f
동작:
1. 오른쪽 끝에서 스폰되어 왼쪽으로 이동
2. 화면 밖으로 나가면 오른쪽 끝으로 재배치 (무한 스크롤)
3. 발판 사이 간격이 "구멍"이 됨
