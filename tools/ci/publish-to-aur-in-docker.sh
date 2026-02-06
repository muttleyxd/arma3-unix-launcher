#!/usr/bin/env bash
set -euxo pipefail

# Script to publish to AUR inside Docker container (needs makepkg)
# Usage: publish-to-aur-in-docker.sh <workspace_dir> <pkgrel> <ssh_dir>

WORKSPACE_DIR="${1:?Workspace directory required}"
PKGREL="${2:?Package release number required}"
SSH_DIR="${3:?SSH directory required}"

echo "=== Publishing to AUR in Docker ==="

docker run --rm \
  --user root \
  -v "$WORKSPACE_DIR:/workspace" \
  -v "$SSH_DIR:/root/.ssh:ro" \
  a3ul_archlinux_build \
  bash -c "
    set -euxo pipefail
    cd /workspace
    git config --global user.name 'GitHub Actions'
    git config --global user.email 'actions@github.com'
    git config --global --add safe.directory '*'
    tools/ci/publish-to-aur.sh /workspace $PKGREL
  "

echo "=== AUR publish complete ==="
