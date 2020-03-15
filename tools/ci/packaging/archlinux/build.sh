#!/usr/bin/env bash
set -euxo pipefail

mkdir /tmp/build
cd /tmp/build
cp /build/PKGBUILD ./
makepkg
cp /tmp/build/*.tar.xz /build
