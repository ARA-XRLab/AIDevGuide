# workflow_manager.py
# Content/Python 폴더에 저장
 
import json
import os
import copy
import random
 
class WorkflowManager:
    """
    워크플로우 템플릿 관리 클래스
    
    설계 원칙:
    - 템플릿 패턴: 공통 구조를 재사용, 가변 부분만 교체
    - 불변성: 원본 템플릿은 수정하지 않음 (deep copy 사용)
    - 확장성: 새 템플릿 추가가 용이한 구조
    """
    
    def __init__(self, templates_dir: str):
        """
        Args:
            templates_dir: 워크플로우 JSON 파일이 있는 디렉토리
        """
        self.templates_dir = templates_dir
        self.templates = {}  # {템플릿명: 워크플로우 dict}
        self._load_templates()
    
    def _load_templates(self):
        """템플릿 디렉토리에서 모든 JSON 워크플로우 로드"""
        if not os.path.exists(self.templates_dir):
            os.makedirs(self.templates_dir)
            return
            
        for filename in os.listdir(self.templates_dir):
            if filename.endswith('.json'):
                template_name = filename[:-5]  # .json 제거
                filepath = os.path.join(self.templates_dir, filename)
                with open(filepath, 'r', encoding='utf-8') as f:
                    self.templates[template_name] = json.load(f)
    
    def get_workflow(self, template_name: str, 
                     positive_prompt: str,
                     negative_prompt: str = "",
                     seed: int = -1,
                     width: int = 512,
                     height: int = 512) -> dict:
        """
        템플릿 기반으로 완성된 워크플로우 생성
        
        Args:
            template_name: 사용할 템플릿 이름 (확장자 제외)
            positive_prompt: 원하는 이미지 특성
            negative_prompt: 피해야 할 특성
            seed: 랜덤 시드 (-1이면 자동 생성)
            width, height: 출력 이미지 크기
            
        Returns:
            파라미터가 주입된 완성 워크플로우
        """
        if template_name not in self.templates:
            raise ValueError(f"템플릿 '{template_name}'을 찾을 수 없습니다.")
        
        # 중요: deep copy로 원본 보존
        workflow = copy.deepcopy(self.templates[template_name])
        
        # 시드 설정
        actual_seed = seed if seed >= 0 else random.randint(0, 2**32 - 1)
        
        # 노드별 파라미터 업데이트
        for node_id, node_data in workflow.items():
            class_type = node_data.get('class_type', '')
            inputs = node_data.get('inputs', {})
            
            # CLIP Text Encode 노드 (프롬프트)
            if class_type == 'CLIPTextEncode':
                # 노드 ID나 이름으로 positive/negative 구분
                if 'positive' in node_id.lower() or node_id == '6':
                    inputs['text'] = positive_prompt
                elif 'negative' in node_id.lower() or node_id == '7':
                    inputs['text'] = negative_prompt or "blurry, low quality"
            
            # KSampler 노드 (시드)
            elif class_type == 'KSampler':
                inputs['seed'] = actual_seed
            
            # EmptyLatentImage 노드 (이미지 크기)
            elif class_type == 'EmptyLatentImage':
                inputs['width'] = width
                inputs['height'] = height
        
        return workflow
