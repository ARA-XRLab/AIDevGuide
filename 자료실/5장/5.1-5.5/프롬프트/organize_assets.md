# 역할 
당신은 10년 경력의 언리얼 엔진 파이썬 전문가입니다. 
## 조건 
- 한국어 주석 
- 코드펜스 없이 출력 
- 에러 처리는 try-except 사용

## 요구사항
organize_assets.py 파일을 만들고
언리얼 에디터에서 선택한 에셋들을 타입별로 자동 정리하는 파이썬 스크립트를 작성해줘.

## 요구사항
- Content Browser에서 선택된 에셋들을 가져와서 분류
- 각 에셋 타입에 맞는 폴더로 자동 이동
- 대상 폴더가 존재하지 않으면 자동 생성
- 이미 올바른 폴더에 있는 에셋은 이동하지 않고 스킵
- 매핑되지 않은 에셋 클래스는 스킵 처리
- 각 에셋별 처리 결과 로그 출력 ([이동 완료], [스킵], [이동 실패])
- 최종 이동된 에셋 개수 요약 출력

## 에셋 타입별 폴더 매핑
다음과 같이 에셋 클래스와 대상 폴더를 매핑:
"StaticMesh": "/Game/Meshes"
"SkeletalMesh": "/Game/Meshes/Skeletal"
"Material": "/Game/Materials"
"MaterialInstanceConstant": "/Game/Materials/Instances"
"Texture2D": "/Game/Textures"
"SoundWave": "/Game/Audio"
"Blueprint": "/Game/Blueprints"
"ParticleSystem": "/Game/Effects"
"NiagaraSystem": "/Game/Effects/Niagara"
