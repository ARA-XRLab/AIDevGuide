@echo off
:: 1. 한글 깨짐 방지 및 환경 설정
chcp 65001 >nul
setlocal enabledelayedexpansion

echo.
echo  [ Unreal Insights - 스마트 내보내기 매니저 ]
echo  ──────────────────────────────────────────────────────

:: ?? 설정 (여기만 수정하면 됩니다!)
set "INSIGHTS_EXE=E:\UE\UE_5.7\Engine\Binaries\Win64\UnrealInsights.exe"
set "TARGET_DIR=E:\SpaceShooter\Documents\Profiling"
set "TEMP_RSP=%TARGET_DIR%\dynamic_export.rsp"

:: 2. 입력 파일 체크
if "%~1"=="" (
    echo  ? [에러] .utrace 파일을 드래그해서 놓아주세요.
    pause
    exit /b
)

set "TRACE_PATH=%~1"
set "TRACE_NAME=%~n1"
set "LOG_PATH=%TARGET_DIR%\Insights_%TRACE_NAME%.log"

:: 3. 대상 폴더 생성
if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"

:: 4. ?? 핵심: export.rsp 파일 즉석 생성
:: 현재 설정된 TARGET_DIR 경로를 기반으로 명령어를 구성합니다.
set "SAFE_DIR=%TARGET_DIR:\=/%"

echo TimingInsights.ExportThreads "%SAFE_DIR%/Threads.csv" > "%TEMP_RSP%"
echo TimingInsights.ExportTimers "%SAFE_DIR%/Timers.csv" >> "%TEMP_RSP%"
echo TimingInsights.ExportTimerStatistics "%SAFE_DIR%/TimerStats.csv" >> "%TEMP_RSP%"

echo  ?? 분석 대상 : %TRACE_NAME%
echo  ?? 저장 경로 : %TARGET_DIR%
echo  ?? 응답 파일 생성 완료!
echo.
echo  ?? 데이터 추출 중... (분석이 끝나면 폴더가 자동으로 열립니다)

:: 5. Unreal Insights 실행
"%INSIGHTS_EXE%" ^
  -OpenTraceFile="%TRACE_PATH%" ^
  -ABSLOG="%LOG_PATH%" ^
  -NoUI ^
  -AutoQuit ^
  -ExecOnAnalysisCompleteCmd="@=%TEMP_RSP%"

:: 6. 결과 알림 및 폴더 열기
if %ERRORLEVEL% EQU 0 (
    echo.
    echo  ──────────────────────────────────────────────────────
    echo  ? 모든 작업이 성공적으로 끝났습니다!
    echo  ?? %TARGET_DIR% 폴더를 확인하세요.
    echo  ──────────────────────────────────────────────────────
    
    timeout /t 1 >nul
    explorer "%TARGET_DIR%"
) else (
    echo.
    echo  ? 에러 발생! 로그를 확인해 주세요.
    pause
)

:: (선택 사항) 임시로 만든 rsp 파일을 지우고 싶다면 아래 주석 해제
:: del "%TEMP_RSP%"

exit /b