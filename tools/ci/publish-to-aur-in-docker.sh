#!/usr/bin/env bash
set -euxo pipefail

# Script to publish to AUR inside Docker container (needs makepkg)
# Usage: publish-to-aur-in-docker.sh <workspace_dir> <pkgrel> <ssh_dir> [docker_image]

WORKSPACE_DIR="${1:?Workspace directory required}"
PKGREL="${2:?Package release number required}"
SSH_DIR="${3:?SSH directory required}"
DOCKER_IMAGE="${4:-a3ul_archlinux_build}"

echo "=== Publishing to AUR in Docker ==="
echo "Using Docker image: $DOCKER_IMAGE"

docker run --rm \
  --user root \
  -v "$WORKSPACE_DIR:/workspace" \
  -v "$SSH_DIR:/ssh-input:ro" \
  "$DOCKER_IMAGE" \
  bash -c "
    set -euxo pipefail

    # Copy SSH files with correct ownership (SSH is strict about this)
    mkdir -p /root/.ssh
    cp -r /ssh-input/* /root/.ssh/
    chown -R root:root /root/.ssh
    chmod 700 /root/.ssh
    chmod 600 /root/.ssh/*

    cd /workspace
    git config --global user.name 'GitHub Actions'
    git config --global user.email 'actions@github.com'
    git config --global --add safe.directory '*'
    tools/ci/publish-to-aur.sh /workspace $PKGREL
  "

echo "=== AUR publish complete ==="
