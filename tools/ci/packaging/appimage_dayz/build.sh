#!/usr/bin/env bash
set -euxo pipefail

SELF_PATH=`dirname "$(readlink -f $0)"`
cd "$SELF_PATH"

A3UL_DIR="${1:-/arma3-unix-launcher}"
BUILD_DIR="${2:-/tmp/build}"
OUTPUT_DIR="${3:-/build}"
STEAMWORKS_SDK_PATH="${4:-}"

STEAMWORKS_PARAMETER=""
if [ "$STEAMWORKS_SDK_PATH" != "" ]; then
  STEAMWORKS_PARAMETER="-DSTEAMWORKS_SDK_PATH=$STEAMWORKS_SDK_PATH"
fi

source /etc/lsb-release
source /etc/os-release

pushd $A3UL_DIR
  SHORT_HASH=`git rev-parse --verify HEAD | cut -c -7`
  COMMIT_COUNT=`git rev-list HEAD --count`
popd

PKG_DIR="$BUILD_DIR/appimage"

mkdir -p $BUILD_DIR
pushd $BUILD_DIR
  mkdir cmake_build

  pushd cmake_build
    cmake $A3UL_DIR -DBUILD_DAYZ_LAUNCHER=ON -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_CXX_COMPILER=g++-8 -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_CXX_FLAGS="-static-libstdc++" -DRUN_TESTS=ON -DSPDLOG_BUILD_SHARED=OFF -DCMAKE_PREFIX_PATH=/qt/5.15.2/gcc_64 $STEAMWORKS_PARAMETER -DCOMMIT_COUNT=$COMMIT_COUNT
    make -j4
    ctest --output-on-failure
    make install DESTDIR=$PKG_DIR
  popd

  QMAKE=/qt/5.15.2/gcc_64/bin/qmake LD_LIBRARY_PATH=/qt/5.15.2/gcc_64/lib:$PKG_DIR/usr/share/dayz-linux-launcher/lib linuxdeploy --appdir appimage/ --plugin qt --output appimage
  cp -n *.AppImage $OUTPUT_DIR || true # -n flag doesn't seem to work on Docker Ubuntu 16.04 at 24th of May, 2020
popd
