@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0RunPhase2Viewport.ps1" %*
exit /b %ERRORLEVEL%
