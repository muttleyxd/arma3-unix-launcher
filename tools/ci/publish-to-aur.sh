#!/usr/bin/env bash
set -euo pipefail

# Script to publish updated PKGBUILD to AUR
# Requires SSH key for AUR access

WORKSPACE_DIR="${1:?Workspace directory required}"
PKGREL="${2:?Package release number required}"
AUR_REPO="${3:-ssh://aur@aur.archlinux.org/arma3-unix-launcher.git}"

# Validate PKGREL is a positive integer
if ! [[ "$PKGREL" =~ ^[0-9]+$ ]]; then
    echo "Error: PKGREL must be a positive integer, got: $PKGREL" >&2
    exit 1
fi

TEMP_DIR=$(mktemp -d)
cd "$TEMP_DIR"

echo "=== Cloning AUR repository ==="
git clone "$AUR_REPO" aur-package
cd aur-package

echo "=== Updating PKGBUILD ==="
# Copy the PKGBUILD template
cp "$WORKSPACE_DIR/tools/ci/packaging/archlinux/PKGBUILD" .

# Update PKGBUILD for AUR (remove /A3ULPATH placeholder, use proper source)
sed -i "s|/A3ULPATH|\$srcdir/\$pkgname|g" PKGBUILD
sed -i "s|^pkgrel=.*|pkgrel=${PKGREL}|g" PKGBUILD

# Update source to point to GitHub
CURRENT_COMMIT=$(cd "$WORKSPACE_DIR" && git rev-parse HEAD)

# Validate commit hash format (40 character hex string)
if ! [[ "$CURRENT_COMMIT" =~ ^[0-9a-f]{40}$ ]]; then
    echo "Error: Invalid commit hash format: $CURRENT_COMMIT" >&2
    exit 1
fi
cat >> PKGBUILD << 'PKGBUILD_EOF'

source=("$pkgname::git+https://github.com/muttleyxd/arma3-unix-launcher.git#commit=COMMIT_PLACEHOLDER")
sha256sums=('SKIP')
PKGBUILD_EOF

sed -i "s|COMMIT_PLACEHOLDER|$CURRENT_COMMIT|g" PKGBUILD

echo "=== Generating .SRCINFO ==="
makepkg --printsrcinfo > .SRCINFO

echo "=== Committing changes ==="
git config user.name "GitHub Actions"
git config user.email "actions@github.com"
git add PKGBUILD .SRCINFO

COMMIT_COUNT=$(cd "$WORKSPACE_DIR" && git rev-list --count HEAD)
COMMIT_SHORT=$(cd "$WORKSPACE_DIR" && git rev-parse --short HEAD)

git diff --cached --quiet || git commit -m "Update to $COMMIT_COUNT.$COMMIT_SHORT-$PKGREL"

echo "=== Pushing to AUR ==="
git push origin master

echo "=== AUR package published successfully ==="
