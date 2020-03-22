#!/usr/bin/env bash

SELF_PATH=`dirname "$(readlink -f $0)"`
pushd "$SELF_PATH" >/dev/null

#no cache because mirrors get invalidated quickly
docker build --no-cache -t a3ul_archlinux_build -f docker/Dockerfile.a3ul_archlinux_build .
docker build --no-cache -t a3ul_ubuntu-18.04_build -f docker/Dockerfile.a3ul_ubuntu-18.04_build .

docker build --no-cache -t github-release -f docker/Dockerfile.github-release .

popd >/dev/null
