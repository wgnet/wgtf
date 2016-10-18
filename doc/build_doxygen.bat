@ECHO OFF

cd third_party\ngt-doc
git submodule init
git submodule update
cd doxygen
doxygen.exe ..\..\..\doxyfile_win.txt
cd ..\..\..\