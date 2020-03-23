#!/usr/bin/env bash
set -euxo pipefail

TMP_BUILD_DIR="/tmp/build"

source /etc/lsb-release
source /etc/os-release

pushd /arma3-unix-launcher
  SHORT_HASH=`git rev-parse --verify HEAD | cut -c -7`
  BRANCH=`git branch | grep "*" | cut -c 3-`
  COMMIT_COUNT=`git rev-list HEAD --count`
popd

PKG_DIR="$TMP_BUILD_DIR/arma3-unix-launcher_$BRANCH-$COMMIT_COUNT-$SHORT_HASH-$ID-$VERSION_ID-amd64"
mkdir -p $PKG_DIR/DEBIAN

pushd $PKG_DIR
  sed "s/VERSION/$COMMIT_COUNT-$SHORT_HASH/g" /build/control >./DEBIAN/control
popd

pushd $TMP_BUILD_DIR
  mkdir cmake_build

  pushd cmake_build
    cmake /arma3-unix-launcher -DCMAKE_CXX_COMPILER=g++-8 -DCMAKE_INSTALL_PREFIX=/usr -DRUN_TESTS=ON
    make -j4
    ctest --output-on-failure
    make install DESTDIR=$PKG_DIR
  popd

  dpkg-deb --build $PKG_DIR
  cp *.deb /build
popd
