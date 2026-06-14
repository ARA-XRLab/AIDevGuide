# 요구사항
ComfyUI 이미지 생성 기능을 MCP 도구로 제공하는 서버를 작성해 주세요.
아래 모듈이 modules 폴더에 이미 존재합니다.
- `comfyui_helper.py`의 `ComfyUIHelper`: ComfyUI 통신 담당
- `workflow_manager.py`의 `WorkflowManager`: 워크플로우 템플릿 관리 담당

## 작업 목표
‘Tools/MCP/comfyui_mcp_server.py` 파일에 FastMCP 서버와 아래 도구 함수들을 작성해 주세요.

## 도구 함수 (@mcp.tool)
1. 함수 이름은 **generate_texture** 입니다.
   - WorkflowManager로 워크플로우를 만들고 ComfyUIHelper로 이미지를 생성합니다
   - 생성 실패 시 오류 메시지를 반환합니다

2. 사용 가능한 워크플로우 템플릿 목록을 반환하는 함수 만들어 주세요.

3. 생성된 이미지 파일 목록을 반환하는 함수 만들어 주세요.

## 실행 예시
MCP 서버를 stdio 방식으로 실행해 주세요.

## 조건
- 한국어 주석
- 코드펜스 없이 전체 코드를 출력
