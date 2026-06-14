# 요구사항
'test_step2.py'를 만들고 WorkflowManager와 ComfyUIHelper를 통합 테스트하는 함수 작성.
- WorkflowManager와 ComfyUIHelper 클래스의 코드는 참조만하고 절대 변경하지 말것.
- 워크플로우 경로: 스크립트와 같은 위치의 workflows 폴더 경로
- get_workflow() 호출 시 파라미터:
  - positive_prompt: "seamless metal texture, sci-fi, 8k, tileable"
  - negative_prompt: "blurry, text, watermark"
  - width: 512, height: 512, seed: 42
- 서버 주소: 127.0.0.1:8188
- 출력 폴더: 스크립트와 같은 위치의 output
## 출력조건
콘솔에 다음 결과 출력
- 테스트명: "STEP 2 테스트: WorkflowManager + ComfyUIHelper"
- 이미지 생성 성공 여부
