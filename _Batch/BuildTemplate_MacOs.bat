@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: CONFIG — edit SCONS_JOBS to match your CPU core count
:: ============================================================
pushd "%~dp0.."
set ROOT_DIR=%CD%
popd
set OUTPUT_DIR=%ROOT_DIR%\export_templates
set LOG_FILE=%OUTPUT_DIR%\build_log_macos.txt
set VERSION_STRING=4.6.stable.custom_build
set SCONS_JOBS=8
set CONFIG_FILE=%~dp0osxcross_config.json
:: ============================================================

echo.
echo ============================================================
echo  Godot macOS Export Template Builder (WSL2 + OSXCross)
echo ============================================================
echo.

:: ── Check WSL ─────────────────────────────────────────────────────────────
wsl --status >nul 2>&1
if !ERRORLEVEL! neq 0 (
    echo [ERROR] WSL2 is not available.
    echo  Run OSXCross_Manager.py to set up your environment.
    goto :done
)

:: ── Read config ───────────────────────────────────────────────────────────
set OXPATH=~/osxcross
set OSXCROSS_SDK=
if exist "%CONFIG_FILE%" (
    for /f "usebackq tokens=*" %%V in (
        `powershell -NoProfile -Command "(Get-Content -Raw '%CONFIG_FILE%' | ConvertFrom-Json).osxcross_wsl_path"`
    ) do if not "%%V"=="" set OXPATH=%%V

    for /f "usebackq tokens=*" %%V in (
        `powershell -NoProfile -Command "(Get-Content -Raw '%CONFIG_FILE%' | ConvertFrom-Json).osxcross_sdk"`
    ) do if not "%%V"=="" set OSXCROSS_SDK=%%V
)

:: ── Expand ~ in OXPATH via WSL ────────────────────────────────────────────
for /f "tokens=*" %%E in ('wsl bash -c "echo %OXPATH%"') do set OX=%%E

:: ── Verify OSXCross exists ────────────────────────────────────────────────
wsl bash -c "test -d \"%OX%/target/bin\"" >nul 2>&1
if !ERRORLEVEL! neq 0 (
    echo [ERROR] OSXCross not found at: %OX%
    echo  Run OSXCross_Manager.py and click "Build OSXCross".
    goto :done
)

:: ── Auto-detect darwin SDK version if not in config ───────────────────────
if "!OSXCROSS_SDK!"=="" (
    for /f "tokens=*" %%D in (
        'wsl bash -c "ls \"%OX%/target/bin/\"*-apple-*-clang 2>/dev/null | head -1 | sed \"s/.*-apple-\(darwin[0-9]*\)-.*/\1/\""'
    ) do set OSXCROSS_SDK=%%D
)

if "!OSXCROSS_SDK!"=="" (
    echo [ERROR] Could not detect OSXCross darwin SDK version.
    echo  Open OSXCross_Manager.py, check the Darwin SDK override field.
    goto :done
)

:: ── Convert Windows paths to WSL paths ───────────────────────────────────
for /f "tokens=*" %%P in ('wsl wslpath -u "%ROOT_DIR:\=/%"') do set WSL_SRC=%%P
for /f "tokens=*" %%P in ('wsl wslpath -u "%OUTPUT_DIR:\=/%"') do set WSL_OUT=%%P

:: ── Build lipo path from known naming convention ─────────────────────────
set LIPO=%OX%/target/bin/x86_64-apple-%OSXCROSS_SDK%-lipo
wsl bash -c "test -f \"%LIPO%\"" >nul 2>&1
if !ERRORLEVEL! neq 0 (
    echo [ERROR] lipo not found at: %LIPO%
    echo  Expected name: x86_64-apple-!OSXCROSS_SDK!-lipo
    echo  Check your OSXCross build or set Darwin SDK Override in OSXCross_Manager.py.
    goto :done
)

:: ── Create output dir, start log ─────────────────────────────────────────
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
echo Build started: %DATE% %TIME% > "%LOG_FILE%"
echo OSXCross: %OX% >> "%LOG_FILE%"
echo SDK: %OSXCROSS_SDK% >> "%LOG_FILE%"
echo Version: %VERSION_STRING% >> "%LOG_FILE%"

echo  OSXCross : %OX%
echo  SDK      : %OSXCROSS_SDK%
echo  Source   : %ROOT_DIR%
echo  Output   : %OUTPUT_DIR%
echo  Jobs     : %SCONS_JOBS%
echo.

:: ── Builds ───────────────────────────────────────────────────────────────
call :scons_build template_release x86_64 [1/4] debug_symbols=no
if !ERRORLEVEL! neq 0 goto :done

call :scons_build template_release arm64  [2/4] debug_symbols=no
if !ERRORLEVEL! neq 0 goto :done

call :scons_build template_debug   x86_64 [3/4]
if !ERRORLEVEL! neq 0 goto :done

call :scons_build template_debug   arm64  [4/4]
if !ERRORLEVEL! neq 0 goto :done

:: ── Lipo — create universal binaries ─────────────────────────────────────
echo.
echo [5/6] Creating universal binaries...
echo [5/6] Creating universal binaries >> "%LOG_FILE%"

wsl bash -c ""%LIPO%" -create "%WSL_SRC%/bin/godot.macos.template_release.x86_64" "%WSL_SRC%/bin/godot.macos.template_release.arm64" -output "%WSL_SRC%/bin/godot.macos.template_release.universal""
if !ERRORLEVEL! neq 0 ( echo [ERROR] lipo failed for template_release. & goto :done )

wsl bash -c ""%LIPO%" -create "%WSL_SRC%/bin/godot.macos.template_debug.x86_64" "%WSL_SRC%/bin/godot.macos.template_debug.arm64" -output "%WSL_SRC%/bin/godot.macos.template_debug.universal""
if !ERRORLEVEL! neq 0 ( echo [ERROR] lipo failed for template_debug. & goto :done )

wsl bash -c "chmod +x "%WSL_SRC%/bin/godot.macos.template_release.universal" "%WSL_SRC%/bin/godot.macos.template_debug.universal""

echo [OK] Universal binaries created.

:: ── Collect ───────────────────────────────────────────────────────────────
echo.
echo [6/6] Collecting and packaging...
echo [6/6] Collecting and packaging >> "%LOG_FILE%"

set REL_SRC=%ROOT_DIR%\bin\godot.macos.template_release.universal
set DBG_SRC=%ROOT_DIR%\bin\godot.macos.template_debug.universal
set REL_DST=%OUTPUT_DIR%\godot.macos.template_release.universal
set DBG_DST=%OUTPUT_DIR%\godot.macos.template_debug.universal

if not exist "%REL_SRC%" ( echo [ERROR] Release binary not found: %REL_SRC% & goto :done )
if not exist "%DBG_SRC%" ( echo [ERROR] Debug binary not found: %DBG_SRC% & goto :done )

copy "%REL_SRC%" "%REL_DST%" >nul
copy "%DBG_SRC%" "%DBG_DST%" >nul
echo %VERSION_STRING%> "%OUTPUT_DIR%\version.txt"
echo [OK] Binaries copied.

:: ── Package into .tpz ────────────────────────────────────────────────────
set ZIP_FILE=%ROOT_DIR%\macos_templates.zip
set TPZ_FILE=%ROOT_DIR%\macos_templates.tpz

if exist "%ZIP_FILE%" del "%ZIP_FILE%"
if exist "%TPZ_FILE%" del "%TPZ_FILE%"

powershell -NoProfile -Command "Compress-Archive -Force -Path '%REL_DST%','%DBG_DST%','%OUTPUT_DIR%\version.txt' -DestinationPath '%ZIP_FILE%'"
if !ERRORLEVEL! neq 0 ( echo [ERROR] Packaging FAILED. & goto :done )

rename "%ZIP_FILE%" "macos_templates.tpz"
echo [OK] Created: %TPZ_FILE%

echo Build finished: %DATE% %TIME% >> "%LOG_FILE%"

:: ── Summary ──────────────────────────────────────────────────────────────
echo.
echo ============================================================
echo  BUILD COMPLETE
echo ============================================================
echo  Release: %REL_DST%
echo  Debug:   %DBG_DST%
echo  Package: %TPZ_FILE%
echo  Log:     %LOG_FILE%
echo.
echo  Editor ^> Manage Export Templates ^> Install from file
echo  Then select: %TPZ_FILE%
echo.

:done
echo.
pause
endlocal
goto :eof

:: ────────────────────────────────────────────────────────────────────────────
:: Subroutine: run one scons build variant via WSL
:: Args: %1=target  %2=arch  %3=step_label  %4=extra_flags (optional)
:: ────────────────────────────────────────────────────────────────────────────
:scons_build
set _T=%~1
set _A=%~2
set _S=%~3
set _X=%~4

echo.
echo %_S% Building %_T% ^(%_A%^)...
echo %_S% Building %_T% (%_A%) >> "%LOG_FILE%"

wsl bash -c "set -e; export OSXCROSS_ROOT='%OX%'; export PATH='%OX%/target/bin':$PATH; cd '%WSL_SRC%'; scons platform=macos target=%_T% arch=%_A% osxcross_sdk=%OSXCROSS_SDK% module_accesskit_enabled=no %_X% -j%SCONS_JOBS%"
set _ERR=!ERRORLEVEL!
if !_ERR! neq 0 (
    echo [ERROR] Build FAILED: %_T% %_A%  ^(exit !_ERR!^)
    echo [ERROR] Build FAILED: %_T% %_A% >> "%LOG_FILE%"
    exit /b 1
)

echo [OK] %_T% %_A%
echo [OK] %_T% %_A% >> "%LOG_FILE%"
exit /b 0
