REM
REM ************ READ IF RUNNING BATCH FILE FOR THE FIRST TIME WITH A NEW QT CREATOR INSTALL ************
REM 
REM When it asks to run cmake, use the following arguments for cmake command. (Change Visual Studio version and QT version as needed)
REM
REM -Wno-dev -G"Visual Studio 12 Win64" -DQT_VERSION=5.6.0 -DBW_CMAKE_TARGET=generic_app_test -DBW_VERIFY_LINUX=OFF
REM
REM Also choose "NMake Generator (Qt 5.3.1)" After this you shouldn't need to add the arguments again.

SET BW_CMAKE_TARGET=generic_app_test
SET BW_VERIFY_LINUX=OFF
SET QML_IMPORT_PATH=%CD%\..\src\core\plugins\plg_qt_common\resources
START /B qtcreator.exe %CD%\..\src\CMakeLists.txt
