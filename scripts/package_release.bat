@echo off
chcp 65001 >nul
title VisionLab Package Release

echo ========================================
echo   VisionLab Package Release
echo ========================================
echo.

cd /d "%~dp0\.."

set VERSION=1.1.0
set PKG=VisionLab-%VERSION%-Windows

if exist dist\%PKG% rmdir /s /q dist\%PKG%
mkdir dist\%PKG%

echo [1/4] Copying executable...
if exist build\bin\Release\VisionLab.exe (
    copy /y build\bin\Release\VisionLab.exe dist\%PKG%\ >nul
) else if exist build\bin\VisionLab.exe (
    copy /y build\bin\VisionLab.exe dist\%PKG%\ >nul
) else (
    echo [ERROR] VisionLab.exe not found! Run build_release.bat first.
    pause
    exit /b 1
)

echo [2/4] Copying docs...
copy /y README.md dist\%PKG%\ >nul
if exist docs mkdir dist\%PKG%\docs 2>nul
xcopy /y /s docs dist\%PKG%\docs\ >nul 2>nul

echo [3/4] Creating run.bat...
echo @echo off > dist\%PKG%\启动.bat
echo cd /d "%%~dp0" >> dist\%PKG%\启动.bat
echo start VisionLab.exe >> dist\%PKG%\启动.bat

echo [4/4] Creating zip...
cd dist
powershell -Command "Compress-Archive -Path '%PKG%' -DestinationPath '%PKG%.zip' -Force"
cd ..

echo.
echo ========================================
echo   Package complete!
echo ========================================
echo.
echo Output: dist\%PKG%.zip
echo.
pause
