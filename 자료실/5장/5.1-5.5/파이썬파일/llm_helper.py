import json
import os
import urllib.request

class LLMHelper:
    """Helper for calling LLM APIs via REST (OpenAI)."""

    def __init__(self):
        # 에디터 스크립트에서 사용할 API 키를 환경 변수에서 읽습니다.
        self.api_key = os.getenv("OPENAI_API_KEY", "")
        if not self.api_key:
            raise RuntimeError("OPENAI_API_KEY is not set")
        # Chat Completions REST 엔드포인트입니다.
        self.base_url = "https://api.openai.com/v1/chat/completions"

    def chat(self, system_prompt, user_message, model="gpt-4o", max_tokens=2048):
        # 호출부를 단순하게 유지하기 위한 래퍼입니다.
        return self.chat_openai(system_prompt, user_message, model, max_tokens)

    def chat_openai(self, system_prompt, user_message, model="gpt-4o", max_tokens=2048):
        # OpenAI Chat Completions 요청 페이로드를 구성합니다.
        payload = {
            "model": model or "gpt-4o",
            "messages": [
                {"role": "system", "content": system_prompt},
                {"role": "user", "content": user_message},
            ],
            "max_tokens": int(max_tokens),
        }

        # JSON을 OpenAI API로 POST합니다.
        data = json.dumps(payload).encode("utf-8")
        req = urllib.request.Request(
            self.base_url,
            data=data,
            headers={
                "Content-Type": "application/json",
                "Authorization": f"Bearer {self.api_key}",
            },
            method="POST",
        )

        # 전체 응답 본문을 읽습니다.
        with urllib.request.urlopen(req, timeout=60) as resp:
            body = resp.read().decode("utf-8")

        # 응답에서 어시스턴트 content를 추출합니다.
        response = json.loads(body)
        return response["choices"][0]["message"]["content"]
