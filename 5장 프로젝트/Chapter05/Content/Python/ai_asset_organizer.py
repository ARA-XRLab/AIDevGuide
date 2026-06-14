import json
import sys
import unreal

sys.path.append(unreal.Paths.project_content_dir() + "Python")
from llm_helper import LLMHelper


SYSTEM_PROMPT = """# 역할
당신은 10년 경력의 Unreal Engine 테크니컬 아티스트입니다.
당신은 게임 프로젝트의 에셋 관리 전문가이며,
Epic Games의 에셋 명명 규칙과 폴더 구조를 잘 알고 있습니다.

# 컨텍스트
Unreal Engine 프로젝트의 에셋을 정리하는 작업을 하고 있습니다.
에셋 이름을 분석하여 적절한 폴더 경로를 추천해야 합니다.

# Unreal Engine 에셋 명명 규칙
- SM_: Static Mesh
- SK_: Skeletal Mesh
- T_: Texture
- M_: Material
- MI_: Material Instance

# 출력 형식
반드시 아래 JSON 형식으로만 응답하세요. 코드 펜스(예: ```json)나 따옴표로 감싸지 마세요.
{
    "assets": [
        {
            "original_name": "원본 에셋 이름",
            "recommended_path": "/Game/추천/폴더/경로",
            "suggested_rename": "권장 새 이름 (필요한 경우)",
            "reason": "추천 이유"
        }
    ]
}
"""

def analyze_and_recommend(asset_names: list) -> dict:
    llm = LLMHelper()
    user_message = f"""다음 에셋들의 적절한 폴더 경로를 추천해주세요:
에셋 목록: {json.dumps(asset_names, ensure_ascii=False, indent=2)}"""
    result = llm.chat(SYSTEM_PROMPT, user_message)
    return result


selected_assets = unreal.EditorUtilityLibrary.get_selected_assets()
selected_asset_names = [asset.get_name() for asset in selected_assets]
result = analyze_and_recommend(selected_asset_names)
print(result)
