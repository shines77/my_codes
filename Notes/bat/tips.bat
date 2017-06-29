
@echo off
@setlocal enableextensions
@set cur_dir="%~dp0"
@cd /d "%cur_dir%"
@echo %cur_dir%
@certutil -addstore root ca.cert.pem
if %ERRORLEVEL% EQU 0 @echo Get some errors, not ok.
pause
