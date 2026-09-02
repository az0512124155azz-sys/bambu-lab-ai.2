@echo off
setlocal
title LayerMind 3D Installer
echo Installing LayerMind 3D for Bambu Studio...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Install-LayerMind-3D.ps1" -Source "%~dp0.."
if errorlevel 1 (
  echo.
  echo Installation failed. Keep this window open and copy the error.
  pause
  exit /b 1
)
echo.
echo SUCCESS - LayerMind 3D was installed and a desktop shortcut was created.
pause
