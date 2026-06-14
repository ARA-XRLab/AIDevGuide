def run(payload):
    import json
    import unreal
    from ai_texture_generator import AITextureGenerator

    # 언리얼 위젯에서 전달된 JSON 문자열을 파이썬 딕셔너리로 변환합니다.
    data = json.loads(payload) if isinstance(payload, str) else payload

    # 위젯 입력값을 텍스처 생성기에 전달할 타입으로 정리합니다.
    text = data.get("text", "")
    width = int(data.get("width", 512))
    height = int(data.get("height", 512))
    template = data.get("template", "texture_basic")
    use_llm = data.get("use_llm", True)

    # JSON에서 문자열 형태의 불리언이 넘어오는 경우를 실제 bool 값으로 변환합니다.
    if isinstance(use_llm, str):
        use_llm = use_llm.lower() in ("true", "1", "yes", "y", "on")

    # AITextureGenerator를 실행하고 생성된 텍스처 에셋 경로를 받습니다.
    generator = AITextureGenerator()
    result = generator.generate_texture(
        text,
        width=width,
        height=height,
        template=template,
        use_llm=use_llm
    )

    # 추가 문구 없이 결과만 언리얼 로그에 출력하고 그대로 반환합니다.
    unreal.log(result)
    return result
