#!/usr/bin/env bash
set -euxo pipefail

# Script to rebuild the Arch Linux Docker image and package
# This ensures we use the latest Arch Linux dependencies

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
cd "$SCRIPT_DIR"

WORKSPACE_DIR="${1:-$(pwd)/../..}"
BUILD_DIR="${2:-/tmp/build}"
OUTPUT_DIR="${3:-/tmp/build}"
PKGREL="${4:-1}"

# Step 1: Rebuild the Arch Linux Docker image with latest packages
echo "=== Rebuilding Arch Linux Docker image with latest packages ==="
docker build --no-cache -t a3ul_archlinux_build -f docker/Dockerfile.a3ul_archlinux_build .

# Step 2: Build the launcher package using the fresh image
echo "=== Building launcher package from source ==="
docker run --rm \
    -v "$WORKSPACE_DIR:/github/workspace" \
    a3ul_archlinux_build \
    bash -c "
        set -euxo pipefail
        git config --global --add safe.directory '*'
        cd /github/workspace
        tools/ci/packaging/archlinux/build.sh /github/workspace $BUILD_DIR $OUTPUT_DIR $PKGREL
        ls -lh $OUTPUT_DIR/*.pkg.tar*
    "

echo "=== Build complete ==="
echo "Package location: $OUTPUT_DIR"
