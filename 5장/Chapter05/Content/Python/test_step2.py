# d:\Projects\Unreal\Chapter05\Content\Python\test_step2.py
import os
import sys
from datetime import datetime

# ==================================================
# 경로 설정
# Content/Python 폴더의 python 파일 위치를 기준으로 절대 경로를 구성
# ==================================================
try:
    script_path = os.path.abspath(__file__)
    script_dir = os.path.dirname(script_path)
except NameError:
    script_dir = os.path.abspath(os.getcwd())

# 로컬 모듈 import 경로 추가
if script_dir not in sys.path:
    sys.path.append(script_dir)

try:
    from workflow_manager import WorkflowManager
    from comfyui_helper import ComfyUIHelper
except ImportError as e:
    print(f"[오류] 로컬 모듈 import 실패: {e}")
    print(f"[오류] 현재 sys.path: {sys.path}")
    raise


def test_step2_function(server_address: str, workflows_dir: str, output_dir: str) -> bool:
    """
    STEP 2: WorkflowManager + ComfyUIHelper 통합 테스트
    """
    try:
        print(f"[OK] workflows 폴더 경로: {workflows_dir}")
        print(f"[OK] 출력 폴더 경로: {output_dir}")

        # 1) workflows 폴더 확인
        if not os.path.isdir(workflows_dir):
            print(f"[오류] workflows 폴더를 찾을 수 없습니다: {workflows_dir}")
            return False

        # 2) 출력 폴더 생성
        try:
            os.makedirs(output_dir, exist_ok=True)
            print(f"[OK] 출력 폴더 확인/생성 완료: {output_dir}")
        except OSError as e:
            print(f"[오류] 출력 폴더 생성 실패: {e}")
            return False

        # 3) WorkflowManager 초기화 및 템플릿 확인
        manager = WorkflowManager(workflows_dir)
        template_names = list(manager.templates.keys())
        print(f"[OK] 템플릿 목록(.templates): {template_names}")

        if not template_names:
            print("[오류] 로드된 템플릿이 없습니다.")
            return False

        # 4) 템플릿 선택
        template_name = template_names[0]
        print(f"[OK] 사용 템플릿: {template_name}")

        # 5) 워크플로우 생성
        workflow = manager.get_workflow(
            template_name=template_name,
            positive_prompt="seamless metal texture, sci-fi, 8k, tileable",
            negative_prompt="blurry, text, watermark",
            width=512,
            height=512,
            seed=42
        )
        print("[OK] 워크플로우 생성 완료")

        # 6) ComfyUIHelper 초기화
        helper = ComfyUIHelper(server_address)
        print(f"[OK] ComfyUIHelper 초기화 완료 (서버: {server_address})")

        # 7) 이미지 생성
        generated_files = helper.generate_image(workflow, output_dir)
        if not generated_files:
            print("[오류] 이미지 생성 결과가 없습니다.")
            return False

        print(f"[OK] 생성된 이미지 개수: {len(generated_files)}")

        # 8) 파일 존재 확인
        for file_path in generated_files:
            if os.path.exists(file_path):
                print(f"[OK] 파일 생성 확인: {file_path}")
            else:
                print(f"[오류] 파일이 존재하지 않습니다: {file_path}")
                return False

        print("[성공] STEP 2 통합 테스트 완료")
        return True

    except TimeoutError as e:
        print(f"[오류] ComfyUI 작업 시간 초과: {e}")
        return False
    except Exception as e:
        print(f"[오류] 예기치 못한 오류: {e}")
        import traceback
        traceback.print_exc()
        return False


if __name__ == "__main__":
    print("=" * 50)
    print("STEP 2 테스트: WorkflowManager + ComfyUIHelper")
    print(f"시작 시간: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 50)

    SERVER_ADDRESS = "127.0.0.1:8188"

    workflows_dir_path = os.path.join(script_dir, "workflows")
    output_dir_path = os.path.join(script_dir, "output")

    success = test_step2_function(
        server_address=SERVER_ADDRESS,
        workflows_dir=workflows_dir_path,
        output_dir=output_dir_path
    )

    print("=" * 50)
    if success:
        print("[성공] 결과: 테스트 성공")
    else:
        print("[오류] 결과: 테스트 실패")
    print(f"종료 시간: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 50)
