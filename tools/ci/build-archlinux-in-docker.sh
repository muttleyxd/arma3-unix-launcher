#!/usr/bin/env bash
set -euxo pipefail

# Script to build Arch Linux package inside Docker container
# Usage: build-archlinux-in-docker.sh <workspace_dir> <pkgrel> [docker_image]

WORKSPACE_DIR="${1:?Workspace directory required}"
PKGREL="${2:?Package release number required}"
DOCKER_IMAGE="${3:-a3ul_archlinux_build}"

echo "=== Building Arch Linux package in Docker ==="
echo "Using Docker image: $DOCKER_IMAGE"

docker run --rm \
  -v "$WORKSPACE_DIR:/github/workspace" \
  "$DOCKER_IMAGE" \
  bash -c "
    set -euxo pipefail
    git config --global --add safe.directory '*'
    cd /github/workspace
    tools/ci/packaging/archlinux/build.sh /github/workspace /tmp/build /tmp/build $PKGREL
    ls -lh /tmp/build/*.pkg.tar*
    cp /tmp/build/*.pkg.tar* /github/workspace
  "

echo "=== Build complete ==="
ls -lh "$WORKSPACE_DIR"/*.pkg.tar*
