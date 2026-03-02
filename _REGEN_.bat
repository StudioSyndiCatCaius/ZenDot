@echo off
title Godot Solution Regenerator

echo ========================================
echo    🎯 Godot Solution Regenerator
echo ========================================
echo This will clean, regenerate solution files, and rebuild Godot
echo.

REM Check if we're in a Godot directory
if not exist "SConstruct" (
    echo ❌ Error: Not in a Godot root directory!
    echo Please run this script from your Godot root directory.
    pause
    exit /b 1
)

REM Ask for confirmation
set /p confirm="⚠️  This will clean your build. Continue? (y/N): "
if /i not "%confirm%"=="y" if /i not "%confirm%"=="yes" (
    echo Cancelled.
    pause
    exit /b 0
)

echo.
echo ========================================
echo 🔄 Step 1: Cleaning previous build
echo ========================================
scons --clean
if errorlevel 1 (
    echo ❌ Clean failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo 🔄 Step 2: Regenerating solution files  
echo ========================================
REM Change "windows" to "linuxbsd" or "macos" if needed
scons platform=windows vsproj=yes
if errorlevel 1 (
    echo ❌ Solution generation failed!
    pause
    exit /b 1
)

echo.
set /p build="🔨 Build the editor now? (Y/n): "
if /i "%build%"=="n" goto skip_build
if /i "%build%"=="no" goto skip_build

echo.
echo ========================================
echo 🔄 Step 3: Building Godot editor
echo ========================================
scons platform=windows target=editor dev_build=yes -j4
if errorlevel 1 (
    echo ❌ Build failed, but solution files were regenerated.
) else (
    echo ✅ Build completed successfully!
)

:skip_build
echo.
echo ========================================
echo ✅ Done!
echo ========================================
echo 📁 You can now open the .sln file in JetBrains Rider
echo 🔄 Don't forget to close and reopen Rider!
echo.
pause