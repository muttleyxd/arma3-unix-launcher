#!/usr/bin/env bash
set -euxo pipefail

SELF_PATH=`dirname "$(readlink -f $0)"`
cd "$SELF_PATH"

A3UL_DIR="${1:-/arma3-unix-launcher}"
BUILD_DIR="${2:-/tmp/build}"
OUTPUT_DIR="${3:-/build}"

mkdir -p $BUILD_DIR
cp ./PKGBUILD $BUILD_DIR/PKGBUILD
cd $BUILD_DIR
sed -i "s|/A3ULPATH|$A3UL_DIR|g" PKGBUILD

mkdir src
cp -RL /$A3UL_DIR/* ./src
chown -R builduser:builduser .
rm -rf ./src/build*
makepkg
cp -n /tmp/build/*.tar.* $OUTPUT_DIR
