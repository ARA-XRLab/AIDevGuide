# batch_rename.py
import unreal

def batch_rename_assets(prefix: str, remove_old_prefix: str = ""):
    """
    선택된 에셋들에 접두사를 일괄 추가합니다.
    
    Args:
        prefix: 추가할 새 접두사 (예: "SM_", "T_", "M_")
        remove_old_prefix: 제거할 기존 접두사 (선택사항)
    """
    
    editor_util = unreal.EditorUtilityLibrary
    editor_asset = unreal.EditorAssetLibrary
    
    selected_assets = editor_util.get_selected_assets()
    
    if len(selected_assets) == 0:
        unreal.log_warning("선택된 에셋이 없습니다.")
        return
    
    renamed_count = 0
    
    for asset in selected_assets:
        old_path = asset.get_path_name()
        old_name = asset.get_name()
        
        # 기존 접두사 제거 (지정된 경우)
        new_name = old_name
        if remove_old_prefix and old_name.startswith(remove_old_prefix):
            new_name = old_name[len(remove_old_prefix):]
        
        # 이미 해당 접두사가 있으면 스킵
        if new_name.startswith(prefix):
            unreal.log(f"스킵: {old_name} (이미 접두사 있음)")
            continue
        
        # 새 이름 생성
        new_name = prefix + new_name
        
        # 새 경로 생성 (폴더 경로 + 새 이름)
        directory = old_path.rsplit("/", 1)[0]
        new_path = f"{directory}/{new_name}"
        
        # 이름 변경 실행
        success = editor_asset.rename_asset(old_path, new_path)
        
        if success:
            unreal.log(f"변경 완료: {old_name} → {new_name}")
            renamed_count += 1
        else:
            unreal.log_error(f"변경 실패: {old_name}")
    
    unreal.log(f"총 {renamed_count}개 에셋 이름 변경 완료")

# 실행 예시: Static Mesh에 SM_ 접두사 추가
batch_rename_assets("SM_")