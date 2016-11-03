@ECHO OFF

CALL %~dp0\build_doxygen.bat
CALL %~dp0\build_breathe.bat
CALL %~dp0\build_sphinx.bat