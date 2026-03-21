@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: CONFIG — edit these to match your setup
:: ============================================================
set GODOT_SOURCE=.
set OUTPUT_DIR=export_templates
set VERSION_STRING=4.6.stable.custom_build
set SCONS_JOBS=8
:: ============================================================

set LOG_FILE=%OUTPUT_DIR%\build_log.txt

echo.
echo ============================================================
echo  Godot Windows Export Template Builder
echo ============================================================
echo.

:: Create output dir
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

:: Start log
echo Build started: %DATE% %TIME% > "%LOG_FILE%"
echo Version: %VERSION_STRING% >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

:: ----------------------------------------
:: RELEASE BUILD
:: ----------------------------------------
echo [1/4] Building RELEASE template...
echo [1/4] Building RELEASE template... >> "%LOG_FILE%"
echo.

scons -C "%GODOT_SOURCE%" platform=windows target=template_release arch=x86_64 -j%SCONS_JOBS% 2>&1 | tee -a "%LOG_FILE%"

if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Release build FAILED. Check %LOG_FILE% for details.
    echo [ERROR] Release build FAILED >> "%LOG_FILE%"
    goto :done
)

echo.
echo [OK] Release build complete.
echo [OK] Release build complete >> "%LOG_FILE%"
echo.

:: ----------------------------------------
:: DEBUG BUILD
:: ----------------------------------------
echo [2/4] Building DEBUG template...
echo [2/4] Building DEBUG template... >> "%LOG_FILE%"
echo.

scons -C "%GODOT_SOURCE%" platform=windows target=template_debug arch=x86_64 -j%SCONS_JOBS% 2>&1 | tee -a "%LOG_FILE%"

if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Debug build FAILED. Check %LOG_FILE% for details.
    echo [ERROR] Debug build FAILED >> "%LOG_FILE%"
    goto :done
)

echo.
echo [OK] Debug build complete.
echo [OK] Debug build complete >> "%LOG_FILE%"
echo.

:: ----------------------------------------
:: COLLECT AND RENAME BINARIES
:: ----------------------------------------
echo [3/4] Collecting binaries...
echo [3/4] Collecting binaries... >> "%LOG_FILE%"

set RELEASE_SRC=%GODOT_SOURCE%\bin\godot.windows.template_release.x86_64.exe
set DEBUG_SRC=%GODOT_SOURCE%\bin\godot.windows.template_debug.x86_64.exe
set RELEASE_DST=%OUTPUT_DIR%\windows_release_x86_64.exe
set DEBUG_DST=%OUTPUT_DIR%\windows_debug_x86_64.exe

if not exist "%RELEASE_SRC%" (
    echo [ERROR] Release binary not found at: %RELEASE_SRC%
    echo [ERROR] Release binary not found >> "%LOG_FILE%"
    goto :done
)

if not exist "%DEBUG_SRC%" (
    echo [ERROR] Debug binary not found at: %DEBUG_SRC%
    echo [ERROR] Debug binary not found >> "%LOG_FILE%"
    goto :done
)

copy "%RELEASE_SRC%" "%RELEASE_DST%" >nul
copy "%DEBUG_SRC%"   "%DEBUG_DST%"   >nul

echo [OK] Binaries copied.
echo [OK] Binaries copied >> "%LOG_FILE%"

:: Write version.txt
echo %VERSION_STRING%> "%OUTPUT_DIR%\version.txt"
echo [OK] version.txt written: %VERSION_STRING%
echo [OK] version.txt written >> "%LOG_FILE%"
echo.

:: ----------------------------------------
:: ZIP INTO .TPZ
:: ----------------------------------------
echo [4/4] Packaging into .tpz...
echo [4/4] Packaging into .tpz... >> "%LOG_FILE%"

set ZIP_FILE=windows_templates.zip
set TPZ_FILE=windows_templates.tpz

:: Zip first (PowerShell only supports .zip), then rename to .tpz
if exist "%ZIP_FILE%" del "%ZIP_FILE%"
if exist "%TPZ_FILE%" del "%TPZ_FILE%"

powershell -Command "Compress-Archive -Force -Path '%OUTPUT_DIR%\windows_release_x86_64.exe','%OUTPUT_DIR%\windows_debug_x86_64.exe','%OUTPUT_DIR%\version.txt' -DestinationPath '%ZIP_FILE%'"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Packaging FAILED.
    echo [ERROR] Packaging FAILED >> "%LOG_FILE%"
    goto :done
)

rename "%ZIP_FILE%" "%TPZ_FILE%"

echo [OK] Created: %TPZ_FILE%
echo [OK] Created: %TPZ_FILE% >> "%LOG_FILE%"

:: ----------------------------------------
:: SUMMARY
:: ----------------------------------------
echo.
echo ============================================================
echo  BUILD COMPLETE
echo ============================================================
echo.
echo  Release:  %RELEASE_DST%
echo  Debug:    %DEBUG_DST%
echo  Package:  %TPZ_FILE%
echo  Log:      %LOG_FILE%
echo.
echo  Install in editor:
echo  Editor ^> Manage Export Templates ^> Install from file
echo  Then select: %TPZ_FILE%
echo.

echo Build finished: %DATE% %TIME% >> "%LOG_FILE%"

:done
echo.
pause
endlocal