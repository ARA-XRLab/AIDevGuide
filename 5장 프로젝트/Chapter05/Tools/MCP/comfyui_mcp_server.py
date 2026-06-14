# comfyui_mcp_server.py
# ComfyUI 이미지 생성 기능을 MCP 도구로 노출하는 FastMCP 서버입니다.

from pathlib import Path
import sys
from typing import Any

from mcp.server.fastmcp import FastMCP


# 서버 파일 기준으로 modules/workflows/output 폴더를 고정해 실행 위치가 달라도 동작하게 합니다.
BASE_DIR = Path(__file__).resolve().parent
MODULES_DIR = BASE_DIR / "modules"
WORKFLOWS_DIR = BASE_DIR / "workflows"
OUTPUT_DIR = BASE_DIR / "output"

# 로컬 helper 모듈을 import할 수 있도록 MCP 서버 시작 시점에 모듈 경로를 추가합니다.
sys.path.append(str(MODULES_DIR))

from comfyui_helper import ComfyUIHelper
from workflow_manager import WorkflowManager


mcp = FastMCP("comfyui-texture-generator")


def _get_workflow_manager() -> WorkflowManager:
    """워크플로우 템플릿을 최신 상태로 읽기 위한 WorkflowManager를 생성합니다."""
    return WorkflowManager(str(WORKFLOWS_DIR))


@mcp.tool()
def generate_texture(
    positive_prompt: str,
    negative_prompt: str = "text, watermark, blurry, low quality",
    width: int = 512,
    height: int = 512,
    template: str = "texture_basic",
    seed: int = -1,
    server_address: str = "127.0.0.1:8188",
) -> dict[str, Any]:
    """
    ComfyUI 워크플로우 템플릿으로 텍스처 이미지를 생성합니다.

    Args:
        positive_prompt: 생성할 텍스처의 원하는 특징
        negative_prompt: 제외할 특징
        width: 생성 이미지 너비
        height: 생성 이미지 높이
        template: 사용할 워크플로우 템플릿 이름
        seed: 생성 시드 (-1이면 랜덤)
        server_address: ComfyUI 서버 주소

    Returns:
        생성 성공/실패 상태와 이미지 경로 목록
    """
    try:
        OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

        # WorkflowManager가 템플릿 JSON에 프롬프트, 해상도, 시드를 주입합니다.
        workflow_manager = _get_workflow_manager()
        workflow = workflow_manager.get_workflow(
            template,
            positive_prompt=positive_prompt,
            negative_prompt=negative_prompt,
            seed=seed,
            width=width,
            height=height,
        )

        # ComfyUIHelper가 큐 등록, 완료 대기, 이미지 다운로드를 한 번에 처리합니다.
        comfyui = ComfyUIHelper(server_address)
        image_paths = comfyui.generate_image(workflow, str(OUTPUT_DIR))

        if not image_paths:
            return {
                "success": False,
                "message": "이미지 생성 결과가 없습니다.",
                "images": [],
            }

        return {
            "success": True,
            "message": "이미지 생성이 완료되었습니다.",
            "images": image_paths,
        }
    except Exception as error:
        return {
            "success": False,
            "message": f"ComfyUI 이미지 생성 실패: {error}",
            "images": [],
        }


@mcp.tool()
def list_workflow_templates() -> dict[str, Any]:
    """
    사용 가능한 워크플로우 템플릿 목록을 반환합니다.

    Returns:
        workflows 폴더에 있는 JSON 템플릿 이름 목록
    """
    try:
        workflow_manager = _get_workflow_manager()
        return {
            "success": True,
            "templates": sorted(workflow_manager.templates.keys()),
        }
    except Exception as error:
        return {
            "success": False,
            "message": f"워크플로우 템플릿 목록 조회 실패: {error}",
            "templates": [],
        }


@mcp.tool()
def list_generated_images() -> dict[str, Any]:
    """
    MCP 서버가 생성한 이미지 파일 목록을 반환합니다.

    Returns:
        output 폴더에 저장된 이미지 파일 경로 목록
    """
    image_extensions = {".png", ".jpg", ".jpeg", ".webp"}

    try:
        if not OUTPUT_DIR.exists():
            return {
                "success": True,
                "images": [],
            }

        # 생성 시간을 확인하기 쉽도록 최신 수정 파일이 먼저 오게 정렬합니다.
        image_files = [
            image_path
            for image_path in OUTPUT_DIR.iterdir()
            if image_path.is_file() and image_path.suffix.lower() in image_extensions
        ]
        image_files.sort(key=lambda image_path: image_path.stat().st_mtime, reverse=True)

        return {
            "success": True,
            "images": [str(image_path) for image_path in image_files],
        }
    except Exception as error:
        return {
            "success": False,
            "message": f"생성 이미지 목록 조회 실패: {error}",
            "images": [],
        }


if __name__ == "__main__":
    # MCP 클라이언트에서 stdio transport로 연결할 수 있도록 표준 입출력 방식으로 서버를 실행합니다.
    mcp.run(transport="stdio")
