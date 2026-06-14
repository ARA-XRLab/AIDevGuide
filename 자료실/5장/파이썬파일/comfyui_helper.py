# comfyui_helper.py
# Content/Python 폴더에 저장
 
import json
import urllib.request
import urllib.parse
import time
import os
 
class ComfyUIHelper:
    """
    ComfyUI REST API 통신 헬퍼 클래스
    
    설계 원칙:
    - 단일 책임: ComfyUI API 통신만 담당
    - 에러 처리: 타임아웃, 네트워크 오류 처리
    - 재사용성: 워크플로우에 독립적인 범용 설계
    """
    
    def __init__(self, server_address: str = "127.0.0.1:8188"):
        """
        Args:
            server_address: ComfyUI 서버 주소 (기본: localhost:8188)
        """
        self.server_address = server_address
        self.base_url = f"http://{server_address}"
        
    def queue_prompt(self, workflow: dict) -> str:
        """
        워크플로우를 실행 대기열에 추가
        
        Args:
            workflow: ComfyUI 워크플로우 JSON (dict)
            
        Returns:
            prompt_id: 작업 추적용 고유 ID
            
        Note:
            ComfyUI는 비동기 실행 - 즉시 반환 후 백그라운드 처리
        """
        data = json.dumps({"prompt": workflow}).encode('utf-8')
        req = urllib.request.Request(
            f"{self.base_url}/prompt",
            data=data,
            headers={"Content-Type": "application/json"}
        )
        with urllib.request.urlopen(req) as response:
            result = json.loads(response.read())
            return result['prompt_id']
    
    def get_history(self, prompt_id: str) -> dict:
        """실행 결과 조회 - 완료 시 outputs 포함"""
        url = f"{self.base_url}/history/{prompt_id}"
        with urllib.request.urlopen(url) as response:
            return json.loads(response.read())
    
    def wait_for_completion(self, prompt_id: str, timeout: int = 120) -> dict:
        """
        작업 완료까지 폴링 방식으로 대기
        
        Args:
            prompt_id: 추적할 작업 ID
            timeout: 최대 대기 시간 (초)
            
        Returns:
            작업 결과 (outputs 포함)
            
        Raises:
            TimeoutError: 지정 시간 내 미완료 시
        """
        start_time = time.time()
        while time.time() - start_time < timeout:
            history = self.get_history(prompt_id)
            if prompt_id in history:
                return history[prompt_id]
            time.sleep(1)  # 1초 간격 폴링
        raise TimeoutError("ComfyUI 작업 시간 초과")
    
    def download_image(self, filename: str, subfolder: str, 
                       output_path: str) -> str:
        """
        생성된 이미지를 로컬에 다운로드
        
        Args:
            filename: ComfyUI 출력 파일명
            subfolder: 출력 하위 폴더
            output_path: 저장할 로컬 경로
        """
        params = urllib.parse.urlencode({
            "filename": filename,
            "subfolder": subfolder,
            "type": "output"
        })
        url = f"{self.base_url}/view?{params}"
        urllib.request.urlretrieve(url, output_path)
        return output_path
    
    def generate_image(self, workflow: dict, output_dir: str) -> list:
        """
        고수준 API: 워크플로우 실행부터 이미지 저장까지 일괄 처리
        
        Args:
            workflow: 완성된 워크플로우 JSON
            output_dir: 이미지 저장 디렉토리
            
        Returns:
            생성된 이미지 파일 경로 목록
        """
        # 1. 실행 요청
        prompt_id = self.queue_prompt(workflow)
        
        # 2. 완료 대기
        result = self.wait_for_completion(prompt_id)
        
        # 3. 이미지 다운로드
        output_files = []
        outputs = result.get('outputs', {})
        
        for node_id, node_output in outputs.items():
            if 'images' in node_output:
                for image_info in node_output['images']:
                    filename = image_info['filename']
                    subfolder = image_info.get('subfolder', '')
                    output_path = os.path.join(output_dir, filename)
                    self.download_image(filename, subfolder, output_path)
                    output_files.append(output_path)
        
        return output_files
