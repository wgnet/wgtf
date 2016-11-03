@ECHO OFF

IF NOT EXIST %~dp0\build\doxygen (
	echo.Doxygen required to be built first.
	exit /b 1
)

start cmd /c cd %~dp0\third_party\breathe & git submodule init & git submodule update
python %~dp0\third_party\breathe\breathe-apidoc.py -f -o %~dp0\build\breathe\ %~dp0\build\doxygen\