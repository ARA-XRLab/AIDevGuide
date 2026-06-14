import unreal

def get_asset_info():
    """선택된 에셋들의 상세 정보를 출력합니다."""
    
    editor_util = unreal.EditorUtilityLibrary
    selected_assets = editor_util.get_selected_assets()
    
    if len(selected_assets) == 0:
        unreal.log_warning("선택된 에셋이 없습니다. Content Browser에서 에셋을 선택해주세요.")
        return
    
    unreal.log("=" * 50)
    unreal.log(f"선택된 에셋 수: {len(selected_assets)}")
    unreal.log("=" * 50)
    
    for asset in selected_assets:
        asset_name = asset.get_name()
        asset_class = asset.get_class().get_name()
        asset_path = asset.get_path_name()
        
        unreal.log(f"이름: {asset_name}")
        unreal.log(f"클래스: {asset_class}")
        unreal.log(f"경로: {asset_path}")
        unreal.log("-" * 30)

# 스크립트 실행
get_asset_info()
