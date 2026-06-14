# test_step4.py
import os
from ai_texture_generator import AITextureGenerator
 
def _run_full_pipeline(use_llm: bool = True) -> bool:
    print("=" * 60)
    print("STEP 3: 전체 파이프라인 테스트")
    print("=" * 60)
    
    if not os.getenv("OPENAI_API_KEY"):
        print("[알림] OPENAI_API_KEY 없음 - LLM 없이 테스트")
        return _run_full_pipeline(use_llm=False)
    
    generator = AITextureGenerator()
    
    try:
        paths = generator.generate_texture("우주선 내부 금속 벽", use_llm=use_llm)
        if paths and os.path.exists(paths[0]):
            print(f"\n[성공] {paths[0]}")
            return True
    except Exception as e:
        print(f"[오류] {e}")
    return False
 
if __name__ == "__main__":
    success = _run_full_pipeline(use_llm=True)
    print("STEP 4 통과!" if success else "STEP 4 실패")
