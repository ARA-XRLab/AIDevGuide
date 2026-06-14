# 요구사항
언리얼 엔진 AI 텍스처 생성 파이프라인 클래스를 작성해주세요.
자연어 요청을 받아 LLM으로 프롬프트를 최적화하고, ComfyUI로 이미지를 생성한 뒤
언리얼 엔진 텍스처 에셋으로 임포트하는 전체 파이프라인을 담당합니다.

## 배경
아래 모듈이 Content/Python 폴더에 이미 존재합니다.
- `llm_helper.py`의 `LLMHelper`: LLM 호출 담당
- `comfyui_helper.py`의 `ComfyUIHelper`: ComfyUI 통신 담당
- `workflow_manager.py`의 `WorkflowManager`: 워크플로우 템플릿 관리 담당

## 작업 목표
`ai_texture_generator.py` 파일에 `AITextureGenerator` 클래스를 작성해 주세요.

## 시스템 프롬프트 (`PROMPT_OPTIMIZER_SYSTEM`)
클래스 외부에 문자열 상수로 선언합니다.
1. LLM의 역할: Stable Diffusion 이미지 생성 전문가
2. LLM의 작업: 자연어 요청을 이미지 생성 프롬프트로 변환
3. Few-shot 예시를 포함합니다
  - 예시는 입력(자연어 설명)과 출력(JSON) 쌍으로 구성합니다
4. 출력 규칙을 포함합니다
  - 코드 펜스 없이 JSON만 출력
  - positive: 사용자 요청의 핵심 대상, 배경, 분위기, 스타일을 반영한 키워드 사용
  - negative: 피해야 할 특성 나열

## 메서드
1. 메서드의 이름은 **generate_texture** 입니다.
   - LLM으로 사용자 요청을 최적화된 프롬프트로 변환하고, 워크플로우를 생성한 뒤, ComfyUI로 이미지를 생성하고, 그 결과를 언리얼 엔진 텍스처 에셋으로 임포트하여 경로를 반환합니다.

## 실행 예시
AITextureGenerator로 자연어 설명과 이미지 크기, 템플릿, llm 사용여부를 지정하여 텍스처를 생성해 주세요.

## 조건
- 한국어 주석
- 코드펜스 없이 클래스 전체를 출력
- 임시 이미지 저장 경로는 언리얼 프로젝트의 Saved 폴더 하위로 고정합니다
- 생성된 텍스처는 `/Game/Textures/AI_Generated` 경로에 `T_AI_` 접두사로 자동 임포트 합니다
- 문구는 반드시 한글로 작성
