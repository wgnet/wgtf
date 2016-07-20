@echo off
where py >nul 2>nul
if %ERRORLEVEL% equ 0 (
	@py -2 wg_cmake.py
) else (
	@python wg_cmake.py
)
@pause