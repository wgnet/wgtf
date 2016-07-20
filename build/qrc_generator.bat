@echo off
where py >nul 2>nul
if %ERRORLEVEL% equ 0 (
	@py %~dp0\qrc_generator.py %*
) else (
	@python %~dp0\qrc_generator.py %*
)