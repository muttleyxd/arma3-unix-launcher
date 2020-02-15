#!/usr/bin/env bash
set -euxo pipefail

realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SELF_PATH=`realpath $(dirname $0)`
A3UL_PATH=`realpath $SELF_PATH/../../../..`

BUILD_DIR=/tmp/build_a3ul_mac

mkdir -p $BUILD_DIR
pushd $BUILD_DIR

cmake $A3UL_PATH -DCPACK_GENERATOR=DragNDrop -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_PREFIX_PATH='/usr/local;/usr/local/opt/qt'
make -j4
make package

popd
