@echo off
setlocal enabledelayedexpansion

pushd "%~dp0.."
set ROOT_DIR=%CD%
popd

:: Refresh PATH so gh is visible if recently installed
for /f "tokens=2*" %%A in ('reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path 2^>nul') do set "SYS_PATH=%%B"
for /f "tokens=2*" %%A in ('reg query "HKCU\Environment" /v Path 2^>nul') do set "USR_PATH=%%B"
if defined USR_PATH (set "PATH=%SYS_PATH%;%USR_PATH%") else (set "PATH=%SYS_PATH%")

set GH=gh
where gh >nul 2>&1
if !ERRORLEVEL! neq 0 (
    if exist "%ProgramFiles%\GitHub CLI\gh.exe" (
        set "GH=%ProgramFiles%\GitHub CLI\gh.exe"
    ) else (
        echo [ERROR] GitHub CLI ^(gh^) is not installed or not in PATH.
        echo  Install from: https://cli.github.com
        goto :done
    )
)

set INI=%~dp0CLI_Login.ini

if not exist "%INI%" (
    echo [ERROR] CLI_Login.ini not found at: %INI%
    echo  Create it by copying CLI_Login.ini.example and filling in your token.
    goto :done
)

:: Read TOKEN= line from the ini, skip comment lines starting with #
set TOKEN=
for /f "usebackq tokens=1,* delims==" %%K in ("%INI%") do (
    set "KEY=%%K"
    set "VAL=%%L"
    set "KEY=!KEY: =!"
    if /i "!KEY!"=="TOKEN" set "TOKEN=!VAL: =!"
)

if "%TOKEN%"=="" (
    echo [ERROR] No TOKEN found in CLI_Login.ini.
    echo  Open CLI_Login.ini and replace "paste_your_token_here" with your PAT.
    goto :done
)

if "%TOKEN%"=="paste_your_token_here" (
    echo [ERROR] TOKEN in CLI_Login.ini is still the placeholder.
    echo  Open CLI_Login.ini and replace "paste_your_token_here" with your PAT.
    goto :done
)

echo.
echo ============================================================
echo  GitHub CLI Login
echo ============================================================
echo.
echo  Logging in with token from CLI_Login.ini...
echo.

echo !TOKEN! | %GH% auth login --with-token --hostname github.com

if !ERRORLEVEL! neq 0 (
    echo.
    echo [ERROR] Login FAILED. Your token may be invalid or expired.
    echo  Generate a new one at: GitHub ^> Settings ^> Developer settings ^> Personal access tokens
    goto :done
)

echo.
echo [OK] Logged in successfully.
echo.
%GH% auth status

:done
echo.
pause
endlocal
