# ai_texture_generator.py
# Content/Python 폴더에 저장
# 프롬프트 최적화 시스템 프롬프트 설계
# Part 1 프롬프트 기법 적용: 역할 부여 + Few-shot + 구조화된 출력
 
PROMPT_OPTIMIZER_SYSTEM = """
# 역할
당신은 Stable Diffusion 이미지 생성 전문가입니다.
게임 텍스처 제작에 특화되어 있으며, 
타일링(seamless) 텍스처와 PBR 워크플로우에 정통합니다.
 
# 작업
사용자의 자연어 요청을 Stable Diffusion에 최적화된 
이미지 생성 프롬프트로 변환합니다.
 
# Few-shot 예시 (Part 1 - 1.2.1절 기법)
 
## 예시 1
입력: "우주선 내부 금속 벽 텍스처"
출력:
{
    "positive": "seamless metal wall texture, sci-fi spaceship interior, 
                 brushed aluminum, industrial panels, subtle scratches, 
                 8k, photorealistic, game asset, tileable, PBR ready",
    "negative": "text, watermark, logo, blurry, low resolution, 
                 cartoon, painted, wood, organic"
}
 
## 예시 2  
입력: "판타지 게임용 돌바닥 타일"
출력:
{
    "positive": "seamless stone floor texture, medieval fantasy, 
                 cobblestone, worn edges, moss details, tileable pattern, 
                 game texture, 4k, realistic lighting",
    "negative": "text, watermark, modern, sci-fi, blurry, distorted, 
                 people, objects"
}
 
# 출력 규칙
- 반드시 JSON 형식으로만 응답 (코드 펜스 없이)
- positive: 원하는 특성을 상세히 기술, seamless/tileable 키워드 포함
- negative: 피해야 할 특성 나열
- 게임 텍스처에 적합한 키워드 사용 (PBR, game asset 등)
"""
 

import unreal
import os
import sys
import re
 
sys.path.append(unreal.Paths.project_content_dir() + "Python")
 
from llm_helper import LLMHelper
from comfyui_helper import ComfyUIHelper
from workflow_manager import WorkflowManager
 
class AITextureGenerator:
    """
    AI 텍스처 생성 파이프라인 통합 클래스 (Orchestrator)
    
    설계 원칙:
    - 파사드 패턴: 복잡한 서브시스템을 단순한 인터페이스로 제공
    - 의존성 주입: 각 헬퍼 클래스를 생성자에서 초기화
    - 단일 진입점: generate_texture() 메서드로 전체 파이프라인 실행
    """
    
    def __init__(self):
        """의존성 초기화"""
        # 1. LLM 헬퍼 (프롬프트 최적화용)
        self.llm = LLMHelper()
        
        # 2. ComfyUI 헬퍼 (이미지 생성용)
        self.comfy = ComfyUIHelper("127.0.0.1:8188")
        
        # 3. 워크플로우 매니저 (템플릿 관리)
        templates_dir = os.path.join(
            unreal.Paths.project_content_dir(), 
            "Python", "workflows"
        )
        self.workflow_manager = WorkflowManager(templates_dir)
        
        # 4. 임시 저장 경로
        self.temp_dir = os.path.join(
            unreal.Paths.project_saved_dir(), 
            "GeneratedTextures"
        )
        os.makedirs(self.temp_dir, exist_ok=True)
    
    def optimize_prompt(self, user_request: str) -> dict:
        """
        자연어 요청을 이미지 생성 프롬프트로 변환
    
        Part 1 기법 적용:
        - 역할 부여: SD 전문가 페르소나 (1.2.3절)
        - Few-shot: 2개의 변환 예시 제공 (1.2.1절)
        - 구조화된 출력: JSON 형식 요청 (1.3절)
    
        Args:
            user_request: 사용자의 자연어 요청
        
        Returns:
            {"positive": str, "negative": str} 형태의 dict
        """
        response = self.llm.chat(
            PROMPT_OPTIMIZER_SYSTEM,
            f'입력: "{user_request}"\n출력:'
        )
    
        # JSON 파싱 (코드 펜스 제거 처리)
        import json
        clean_response = response.strip()
        if clean_response.startswith('```'):
            clean_response = clean_response.split('\n', 1)[1]
            clean_response = clean_response.rsplit('```', 1)[0]
    
        return json.loads(clean_response)


    def generate_texture(self, user_request: str, 
                        width: int = 512, 
                        height: int = 512,
                        template: str = "texture_basic", use_llm=True) -> str:
        """
        메인 파이프라인: 자연어 → 텍스처 에셋
        
        Args:
            user_request: 자연어 텍스처 설명
            width, height: 출력 이미지 크기
            template: 워크플로우 템플릿 이름
            
        Returns:
            생성된 Unreal 텍스처 에셋 경로
        """
        unreal.log(f"[AITextureGen] 요청: {user_request}")
        
        try:
            # STEP 1: LLM으로 프롬프트 최적화
            unreal.log("[AITextureGen] Step 1: 프롬프트 최적화...")
            prompts = self.optimize_prompt(user_request) if use_llm else {
                "positive": user_request,
                "negative": "blurry, text, watermark"
            }
            unreal.log(f"[AITextureGen] Positive: {prompts['positive'][:50]}...")
            
            # STEP 2: 워크플로우 생성
            unreal.log("[AITextureGen] Step 2: 워크플로우 생성...")
            workflow = self.workflow_manager.get_workflow(
                template,
                positive_prompt=prompts['positive'],
                negative_prompt=prompts['negative'],
                width=width,
                height=height
            )
            
            # STEP 3: ComfyUI로 이미지 생성
            unreal.log("[AITextureGen] Step 3: 이미지 생성 중...")
            image_paths = self.comfy.generate_image(workflow, self.temp_dir)
            
            if not image_paths:
                raise RuntimeError("이미지 생성 실패")
            
            image_path = image_paths[0]
            unreal.log(f"[AITextureGen] 이미지 생성 완료: {image_path}")
            
            # STEP 4: Unreal Engine에 임포트
            unreal.log("[AITextureGen] Step 4: Unreal 임포트...")
            texture_path = self._import_as_texture(image_path, user_request)
            
            unreal.log(f"[AITextureGen] 완료: {texture_path}")
            return texture_path
                
        except Exception as e:
            unreal.log_error(f"[AITextureGen] 오류: {str(e)}")
            return None
    
    def _import_as_texture(self, image_path: str, description: str) -> str:
        """
        이미지 파일을 Unreal 텍스처 에셋으로 임포트
        
        사용 API:
        - unreal.AssetImportTask: 임포트 작업 정의
        - unreal.AssetToolsHelpers: 실제 임포트 실행
        """
        # 안전한 에셋 이름 생성
        safe_name = re.sub(r'[^a-zA-Z0-9]', '_', description)[:30]
        asset_name = f"T_AI_{safe_name}"
        
        folder_path = "/Game/Textures/AI_Generated"
        destination_path = f"{folder_path}/{asset_name}"
        
        # 폴더 생성
        editor_asset = unreal.EditorAssetLibrary
        if not editor_asset.does_directory_exist(folder_path):
            editor_asset.make_directory(folder_path)
        
        # 임포트 태스크 설정
        import_task = unreal.AssetImportTask()
        import_task.filename = image_path
        import_task.destination_path = folder_path
        import_task.destination_name = asset_name
        import_task.replace_existing = True
        import_task.automated = True
        import_task.save = True
        
        # 임포트 실행
        unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(
            [import_task]
        )
        
        return destination_path
 
# 실행 예시
if __name__ == "__main__":
    generator = AITextureGenerator()
    result = generator.generate_texture(
        "우주 정거장 바닥 금속 타일 텍스처",
        width=1024,
        height=1024
    )
    print(f"생성된 텍스처: {result}")
