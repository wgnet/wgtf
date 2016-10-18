#!/bin/sh
# ************
# READ IF RUNNING SHELL SCRIPT FOR THE FIRST TIME WITH A NEW QT CREATOR INSTALL
# ************
#
# When it asks to run CMake, use the following arguments for cmake command.
# (Change QT version as needed)
#
# -Wno-dev -DQT_VERSION=5.6.0 -DBW_CMAKE_TARGET=generic_app_test -DBW_VERIFY_LINUX=OFF
#
# Choose "Unix Generator (Desktop Qt 5.6.0 clang 64bit)"
# After this you shouldn't need to add the arguments again.

BASEDIR=$(dirname "$0")
BW_CMAKE_TARGET=generic_app_test
BW_VERIFY_LINUX=OFF
QML_IMPORT_PATH=$BASEDIR/../src/core/plugins/plg_qt_common/resources
open -a "Qt Creator" $BASEDIR/../src/CMakeLists.txt
