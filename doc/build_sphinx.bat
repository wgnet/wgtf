@ECHO OFF

if "%SPHINXBUILD%" == "" (
	set SPHINXBUILD=sphinx-build
)

set BUILDDIR=%~dp0/build/sphinx
set ALLSPHINXOPTS=-d %BUILDDIR%/doctrees %SPHINXOPTS% %~dp0

%SPHINXBUILD% -b html %ALLSPHINXOPTS% %BUILDDIR%/html
if errorlevel 1 exit /b 1
echo.
echo.Build finished. The HTML pages are in %BUILDDIR%/html.