#!/bin/sh
BUILD_DIR="$(cd "$(dirname ${0})" && pwd -P )"
python ${BUILD_DIR}/wg_cmake.py "$@"
