# print_selected_assets.py
import unreal
editor_util=unreal.EditorUtilityLibrary
selected_assets=editor_util.get_selected_assets()
if len(selected_assets)==0:
   unreal.log_warning("선택된 애셋이 없습니다.")
else:
   for asset in selected_assets:
       asset_name = asset.get_name()
       unreal.log(f"애셋: {asset_name}")