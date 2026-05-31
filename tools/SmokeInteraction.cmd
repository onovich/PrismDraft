@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0SmokeInteraction.ps1" %*
exit /b %ERRORLEVEL%
