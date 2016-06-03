@echo off
where py >nul 2>nul
if %ERRORLEVEL% equ 0 (
	@py -2 deploy_python.py %*
) else (
	@python deploy_python.py %*
)
@pause
