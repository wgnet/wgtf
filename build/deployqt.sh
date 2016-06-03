#!/bin/sh
export PATH=$1:$PATH
export DYLD_FRAMEWORK_PATH=$1/../lib
APP_BUNDLE_DIR="$(cd ${2} && pwd -P )"
shift;shift
macdeployqt $APP_BUNDLE_DIR $*
