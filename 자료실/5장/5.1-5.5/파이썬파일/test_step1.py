# d:\Projects\Unreal\Chapter05\Content\Python\test_step1.py
import os
import sys
import json
from datetime import datetime

# --- 경로 설정 ---
# 이 스크립트의 절대 경로를 기준으로 경로를 설정합니다.
# 이렇게 하면 어디서 실행하든 동일한 경로 구조를 유지할 수 있습니다.
try:
    script_path = os.path.abspath(__file__)
    script_dir = os.path.dirname(script_path)
except NameError:
    # 대화형 인터프리터에서 실행될 경우를 대비
    script_dir = os.path.abspath(os.getcwd())

# 프로젝트 루트를 기준으로 comfyui_helper 모듈을 임포트하기 위해 경로 추가
# 일반적으로 Content/Python 폴더가 파이썬 실행의 루트가 됩니다.
if script_dir not in sys.path:
    sys.path.append(script_dir)

# 로컬 모듈 임포트
# 경로 설정이 끝난 후 임포트해야 합니다.
try:
    from comfyui_helper import ComfyUIHelper
except ImportError as e:
    print(f"[오류] ComfyUIHelper 모듈을 임포트할 수 없습니다: {e}")
    print(f"       현재 sys.path: {sys.path}")
    sys.exit(1) # 테스트 진행이 불가능하므로 종료


def test_step1_function(server_address: str, workflow_path: str, output_dir: str) -> bool:
    """
    ComfyUIHelper.generate_image 함수를 테스트합니다.

    Args:
        server_address (str): ComfyUI 서버 주소.
        workflow_path (str): 사용할 워크플로우 JSON 파일의 절대 경로.
        output_dir (str): 결과 이미지를 저장할 절대 경로.

    Returns:
        bool: 테스트 성공 시 True, 실패 시 False.
    """
    print(f"[LOG] 워크플로우 파일 경로: {workflow_path}")
    print(f"[LOG] 출력 디렉토리: {output_dir}")

    # 1. 워크플로우 파일 존재 여부 확인
    if not os.path.exists(workflow_path):
        print(f"[오류] 워크플로우 파일을 찾을 수 없습니다: {workflow_path}")
        return False
    
    # 2. 출력 디렉토리 생성 (없을 경우)
    try:
        os.makedirs(output_dir, exist_ok=True)
        print(f"[OK] 출력 디렉토리 확인/생성 완료: {output_dir}")
    except OSError as e:
        print(f"[오류] 출력 디렉토리 생성 실패: {e}")
        return False

    try:
        # 3. 워크플로우 파일 로드
        with open(workflow_path, 'r', encoding='utf-8') as f:
            workflow = json.load(f)
        print("[OK] 워크플로우 JSON 파일 로드 성공")

        # 4. ComfyUIHelper 인스턴스 생성
        helper = ComfyUIHelper(server_address)
        print(f"[OK] ComfyUIHelper 인스턴스 생성 (서버: {server_address})")
        
        # 5. 이미지 생성 함수 호출
        print("[LOG] 이미지 생성을 시작합니다... (시간이 걸릴 수 있습니다)")
        generated_files = helper.generate_image(workflow, output_dir)
        
        # 6. 결과 검증
        if not generated_files:
            print("[오류] 이미지 생성 결과물이 없습니다.")
            return False
        
        print(f"[OK] {len(generated_files)}개의 이미지 파일 정보 수신")
        
        # 7. 실제 파일 존재 여부 확인
        for file_path in generated_files:
            if os.path.exists(file_path):
                print(f"[OK] 파일 생성 확인: {file_path}")
            else:
                print(f"[오류] 파일이 실제로 생성되지 않았습니다: {file_path}")
                return False
        
        print("[성공] 모든 테스트 단계를 통과했습니다.")
        return True

    except FileNotFoundError:
        print(f"[오류] 워크플로우 파일을 찾을 수 없습니다: {workflow_path}")
        return False
    except json.JSONDecodeError:
        print(f"[오류] 워크플로우 파일이 유효한 JSON 형식이 아닙니다: {workflow_path}")
        return False
    except TimeoutError as e:
        print(f"[오류] ComfyUI 작업 시간 초과: {e}")
        return False
    except Exception as e:
        print(f"[오류] 테스트 중 예기치 않은 오류 발생: {e}")
        import traceback
        traceback.print_exc()
        return False


if __name__ == "__main__":
    # ==================================================
    # 테스트 기본 정보 설정
    # ==================================================
    SERVER_ADDRESS = "127.0.0.1:8188"
    WORKFLOW_FILENAME = "texture_basic.json"
    OUTPUT_DIR_NAME = "output"
    
    # 절대 경로 생성
    workflow_file_path = os.path.join(script_dir, "workflows", WORKFLOW_FILENAME)
    output_dir_path = os.path.join(script_dir, OUTPUT_DIR_NAME)
    
    # ==================================================
    print("=" * 50)
    print("STEP 1 테스트: ComfyUIHelper")
    print(f"시작 시간: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 50)
    
    # 테스트 함수 실행
    success = test_step1_function(
        server_address=SERVER_ADDRESS,
        workflow_path=workflow_file_path,
        output_dir=output_dir_path
    )

    print("=" * 50)
    if success:
        print("결과: 테스트 성공")
    else:
        print("결과: 테스트 실패")
    print(f"종료 시간: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 50)
