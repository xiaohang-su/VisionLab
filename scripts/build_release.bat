@echo off
chcp 65001 >nul
title VisionLab Release Builder

echo ========================================
echo   VisionLab Release Build
echo ========================================
echo.

cd /d "%~dp0\.."

if not exist build mkdir build
cd build

echo [1/3] Configuring CMake (Release, ImGui ON)...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DVISIONLAB_UI_IMGUI=ON 2>nul
if errorlevel 1 (
    echo MinGW not found, trying Visual Studio...
    cmake .. -DCMAKE_BUILD_TYPE=Release -DVISIONLAB_UI_IMGUI=ON
    if errorlevel 1 (
        echo.
        echo [ERROR] CMake configure failed!
        echo Please install Visual Studio 2022 or MinGW-w64 + CMake.
        pause
        exit /b 1
    )
)

echo.
echo [2/3] Building...
cmake --build . --config Release -j
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo [3/3] Build complete!
echo.
echo EXE location:
if exist bin\Release\VisionLab.exe (
    echo   %CD%\bin\Release\VisionLab.exe
) else if exist bin\VisionLab.exe (
    echo   %CD%\bin\VisionLab.exe
) else (
    dir /s /b VisionLab.exe 2>nul
)

echo.
echo ========================================
echo   Build Success!
echo ========================================
pause
