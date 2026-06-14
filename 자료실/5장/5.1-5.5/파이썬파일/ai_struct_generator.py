# ai_struct_generator.py
import unreal
import os
import re
import sys
sys.path.append(unreal.Paths.project_content_dir() + "Python")
from llm_helper import LLMHelper
 
# Few-shot 프롬프팅: 예시를 통해 출력 형식 학습
SYSTEM_PROMPT = """# 역할
당신은 Unreal Engine C++ 전문 개발자입니다.
Epic Games 코딩 컨벤션을 철저히 따릅니다.
 
# 작업
자연어 설명을 Unreal Engine C++ 구조체(USTRUCT)로 변환합니다.
 
# Few-shot 예시
 
## 예시 1
입력: "플레이어의 능력치를 저장하는 구조체. 체력, 마나, 공격력, 방어력 포함"
 
출력:
USTRUCT(BlueprintType)
struct FPlayerStats
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Health = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Mana = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackPower = 10.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Defense = 5.0f;
};
 
# 규칙
- 코드 펜스(예: ```cpp)나 따옴표로 감싸지 마세요.
- 파일 맨 위에 필요한 헤더파일 추가 필수.
- generated.h 포함
- USTRUCT(BlueprintType) 매크로 필수
- GENERATED_BODY() 매크로 필수
- 모든 멤버에 UPROPERTY 매크로와 적절한 Category
- 한글 주석으로 각 멤버 설명
## 파일 이름 규칙
- 구조체 이름에서 'F' 접두사를 제외한 이름을 파일명으로 사용 (예: FPlayerStats -> PlayerStats.h)


"""
 
def generate_struct(description: str) -> str:
    llm = LLMHelper()
    user_message = f'입력: "{description}"\n\n출력:'
    response = llm.chat(SYSTEM_PROMPT, user_message)
    save_struct_to_header(response)
    return response

def save_struct_to_header(struct_code: str, filename: str | None = None) -> str:
    # 구조체 코드에서 struct 이름을 뽑아 파일명을 자동으로 결정한다.
    match = re.search(r"\bstruct\s+(F\w+)", struct_code)
    if not match:
        raise ValueError("구조체 이름을 찾을 수 없습니다.")
    struct_name = match.group(1)

    # 저장 경로는 프로젝트 Source/Chapter05 폴더로 고정한다.
    project_dir = unreal.Paths.project_dir()
    target_dir = os.path.join(project_dir, "Source", "Chapter05")
    os.makedirs(target_dir, exist_ok=True)

    # 구조체 이름을 파일명으로 사용한다.
    if filename:
        header_name = filename if filename.endswith(".h") else f"{filename}.h"
    else:
        base_name = struct_name[1:] if struct_name.startswith("F") and len(struct_name) > 1 else struct_name
        header_name = f"{base_name}.h"
    header_path = os.path.join(target_dir, header_name)

    # 생성된 문자열을 그대로 저장한다.
    with open(header_path, "w", encoding="utf-8") as f:
        f.write(struct_code.strip() + "\n")

    return header_path

result = generate_struct("게임 내 아이템의 속성을 저장하는 구조체. 이름, 설명, 무게, 가치 포함")
print(result)   
