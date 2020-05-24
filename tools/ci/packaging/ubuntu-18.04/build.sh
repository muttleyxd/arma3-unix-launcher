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

PKG_DIR="$BUILD_DIR/arma3-unix-launcher-$COMMIT_COUNT.$SHORT_HASH-$ID-$VERSION_ID-amd64"
mkdir -p $PKG_DIR/DEBIAN

sed "s/VERSION/$COMMIT_COUNT-$SHORT_HASH/g" ./control >$PKG_DIR/DEBIAN/control

mkdir -p $BUILD_DIR
pushd $BUILD_DIR
  mkdir cmake_build

  pushd cmake_build
    cmake $A3UL_DIR -DCMAKE_CXX_COMPILER=g++-8 -DCMAKE_INSTALL_PREFIX=/usr -DRUN_TESTS=ON $STEAMWORKS_PARAMETER
    make -j4
    ctest --output-on-failure
    make install DESTDIR=$PKG_DIR
  popd

  dpkg-deb --build $PKG_DIR
  cp -n *.deb $OUTPUT_DIR || true # -n flag doesn't seem to work on Docker Ubuntu 18.04 at 24th of May, 2020
popd
