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
    cmake /arma3-unix-launcher -DCMAKE_CXX_COMPILER=g++-8 -DCMAKE_INSTALL_PREFIX=/opt/arma3-unix-launcher -DCMAKE_CXX_FLAGS="-static-libstdc++" -DRUN_TESTS=ON -DCMAKE_PREFIX_PATH=/qt/5.14.2/gcc_64
    make -j4
    ctest --output-on-failure
    make install DESTDIR=$PKG_DIR
  popd

  pushd $PKG_DIR
    mkdir opt/lib
    pushd opt/lib
      cp -L /qt/5.14.2/gcc_64/lib/libQt5Widgets.so.5 ./
      cp -L /qt/5.14.2/gcc_64/lib/libQt5Gui.so.5 ./
      cp -L /qt/5.14.2/gcc_64/lib/libQt5Core.so.5 ./
      cp -L /qt/5.14.2/gcc_64/lib/libicui18n.so.56 ./
      cp -L /qt/5.14.2/gcc_64/lib/libicuuc.so.56 ./
      cp -L /qt/5.14.2/gcc_64/lib/libicudata.so.56 ./
      cp /arma3-unix-launcher/tools/ci/packaging/ubuntu-16.04/Qt-5.14.2-LICENSE.txt ./
    popd
    mkdir -p usr/bin
    cp /arma3-unix-launcher/tools/ci/packaging/ubuntu-16.04/launch-script.sh usr/bin/arma3-unix-launcher
    chmod +x usr/bin/arma3-unix-launcher
  popd

  dpkg-deb --build $PKG_DIR
  cp *.deb /build
popd
