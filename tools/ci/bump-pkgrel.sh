#!/usr/bin/env bash
set -euo pipefail

# Script to manage pkgrel versioning
# - If commit hash changed: reset pkgrel to 1
# - If commit hash same: increment pkgrel
# Usage: bump-pkgrel.sh <workspace_dir> <previous_package_name>

WORKSPACE_DIR="${1:?Workspace directory required}"
PREVIOUS_PKG="${2:-}"

cd "$WORKSPACE_DIR"

# Get current commit info
CURRENT_COMMIT=$(git rev-parse --short HEAD)
COMMIT_COUNT=$(git rev-list --count HEAD)

echo "Current commit: $CURRENT_COMMIT" >&2
echo "Commit count: $COMMIT_COUNT" >&2

# Determine new pkgrel
NEW_PKGREL=1

if [ -n "$PREVIOUS_PKG" ] && [ -f "$PREVIOUS_PKG" ]; then
    # Extract version from previous package filename
    # Format: arma3-unix-launcher-389.60b26e1-1-x86_64.pkg.tar.zst
    # Version format: COMMIT_COUNT.COMMIT_HASH-PKGREL
    PREV_VERSION=$(echo "$PREVIOUS_PKG" | sed 's/arma3-unix-launcher-//' | sed 's/-x86_64.*//' | cut -d'-' -f1)
    PREV_PKGREL=$(echo "$PREVIOUS_PKG" | sed 's/arma3-unix-launcher-//' | sed 's/-x86_64.*//' | cut -d'-' -f2)
    PREV_COMMIT=$(echo "$PREV_VERSION" | cut -d'.' -f2)

    echo "Previous package: $PREVIOUS_PKG" >&2
    echo "Previous commit: $PREV_COMMIT" >&2
    echo "Previous pkgrel: $PREV_PKGREL" >&2

    if [ "$PREV_COMMIT" = "$CURRENT_COMMIT" ]; then
        # Same commit, increment pkgrel
        NEW_PKGREL=$((PREV_PKGREL + 1))
        echo "Same commit detected, incrementing pkgrel: $PREV_PKGREL -> $NEW_PKGREL" >&2
    else
        # Different commit, reset to 1
        echo "Different commit detected, resetting pkgrel to 1" >&2
        NEW_PKGREL=1
    fi
else
    echo "No previous package found, using pkgrel=1" >&2
fi

echo "New pkgrel: $NEW_PKGREL" >&2
echo "pkgrel=$NEW_PKGREL" > /tmp/pkgrel.env
echo "$NEW_PKGREL"
