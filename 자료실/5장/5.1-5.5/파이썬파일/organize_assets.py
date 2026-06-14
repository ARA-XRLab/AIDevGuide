# organize_assets.py
import unreal
 
# 에셋 클래스별 대상 폴더 매핑
ASSET_FOLDER_MAP = {
    "StaticMesh": "/Game/Meshes",
    "SkeletalMesh": "/Game/Meshes/Skeletal",
    "Material": "/Game/Materials",
    "MaterialInstanceConstant": "/Game/Materials/Instances",
    "Texture2D": "/Game/Textures",
    "SoundWave": "/Game/Audio",
    "Blueprint": "/Game/Blueprints",
    "ParticleSystem": "/Game/Effects",
    "NiagaraSystem": "/Game/Effects/Niagara"
}
 
def organize_selected_assets():
    """선택된 에셋들을 타입에 따라 적절한 폴더로 이동합니다."""
    
    editor_util = unreal.EditorUtilityLibrary
    editor_asset = unreal.EditorAssetLibrary
    
    selected_assets = editor_util.get_selected_assets()
    
    if len(selected_assets) == 0:
        unreal.log_warning("선택된 에셋이 없습니다.")
        return
    
    moved_count = 0
    
    for asset in selected_assets:
        asset_class = asset.get_class().get_name()
        old_path = asset.get_path_name()
        asset_name = asset.get_name()
        
        # 해당 클래스의 대상 폴더 찾기
        target_folder = ASSET_FOLDER_MAP.get(asset_class)
        
        if target_folder is None:
            unreal.log(f"스킵: {asset_name} (매핑 없음: {asset_class})")
            continue
        
        # 대상 폴더가 없으면 생성
        if not editor_asset.does_directory_exist(target_folder):
            editor_asset.make_directory(target_folder)
        
        # 새 경로 생성
        new_path = f"{target_folder}/{asset_name}"
        
        # 이미 대상 폴더에 있으면 스킵
        if old_path.startswith(target_folder):
            unreal.log(f"스킵: {asset_name} (이미 올바른 폴더)")
            continue
        
        # 에셋 이동
        success = editor_asset.rename_asset(old_path, new_path)
        
        if success:
            unreal.log(f"이동 완료: {asset_name} → {target_folder}")
            moved_count += 1
        else:
            unreal.log_error(f"이동 실패: {asset_name}")
    
    unreal.log(f"총 {moved_count}개 에셋 정리 완료")
 
# 실행
organize_selected_assets()
