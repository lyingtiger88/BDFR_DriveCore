@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Build-Editor-UE58.ps1" %*
set "exitCode=%ERRORLEVEL%"
endlocal & exit /b %exitCode%
