@echo off
chcp 65001 >nul
title VisionLab - 一键编译运行

cd /d "%~dp0\.."

echo ========================================
echo   VisionLab 一键编译运行
echo ========================================
echo.

if not exist build\bin\VisionLab.exe if not exist build\bin\Release\VisionLab.exe (
    echo 首次运行，正在编译...
    call scripts\build_release.bat
    if errorlevel 1 exit /b 1
)

echo.
echo 启动 VisionLab...
echo.

if exist build\bin\Release\VisionLab.exe (
    start "" build\bin\Release\VisionLab.exe
) else (
    start "" build\bin\VisionLab.exe
)
