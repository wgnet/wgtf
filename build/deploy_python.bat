@echo off
where py >nul 2>nul
if %ERRORLEVEL% equ 0 (
	@py -2 %~dp0\deploy_python.py %*
) else (
	@python %~dp0\deploy_python.py %*
)
@pause
