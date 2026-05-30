@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0SmokeViewportLifecycle.ps1" %*
exit /b %ERRORLEVEL%
