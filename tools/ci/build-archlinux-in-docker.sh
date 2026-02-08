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
  -e "PKGREL=$PKGREL" \
  --user root \
  "$DOCKER_IMAGE" \
  bash -c "
    set -euxo pipefail

    # Run build as builduser (required by makepkg)
    # builduser can read workspace, writes build output to /tmp/build
    su builduser -c '
      set -euxo pipefail
      git config --global --add safe.directory /github/workspace
      cd /github/workspace
      /github/workspace/tools/ci/packaging/archlinux/build.sh /github/workspace /tmp/build /tmp/build \$PKGREL
      ls -lh /tmp/build/*.pkg.tar*
    '

    # Copy to workspace as root (builduser can't write to mounted volume)
    cp /tmp/build/*.pkg.tar* /github/workspace/
    # Set ownership to match host user
    chown \$(stat -c '%u:%g' /github/workspace) /github/workspace/*.pkg.tar*
  "

echo "=== Build complete ==="
ls -lh "$WORKSPACE_DIR"/*.pkg.tar*
