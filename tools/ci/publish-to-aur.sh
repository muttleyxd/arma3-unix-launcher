#!/usr/bin/env bash
set -euo pipefail

# Script to publish updated PKGBUILD to AUR
# Requires SSH key for AUR access
# This script works with the arma3-unix-launcher-bin AUR package which downloads
# pre-built packages from GitHub releases

WORKSPACE_DIR="${1:?Workspace directory required}"
PKGREL="${2:?Package release number required}"
AUR_REPO="${3:-ssh://aur@aur.archlinux.org/arma3-unix-launcher-bin.git}"

# Validate PKGREL is a positive integer
if ! [[ "$PKGREL" =~ ^[0-9]+$ ]]; then
    echo "Error: PKGREL must be a positive integer, got: $PKGREL" >&2
    exit 1
fi

# Get version info from workspace
COMMIT_COUNT=$(cd "$WORKSPACE_DIR" && git rev-list --count HEAD)
COMMIT_SHORT=$(cd "$WORKSPACE_DIR" && git rev-parse --short HEAD)
GIT_TAG="commit-$COMMIT_COUNT"
PKGVER="${COMMIT_COUNT}.${COMMIT_SHORT}"

echo "=== Version info ==="
echo "Commit count: $COMMIT_COUNT"
echo "Commit short: $COMMIT_SHORT"
echo "Git tag:      $GIT_TAG"
echo "Package ver:  $PKGVER"
echo "Package rel:  $PKGREL"

TEMP_DIR=$(mktemp -d)
cd "$TEMP_DIR"

echo "=== Cloning AUR repository ==="
git clone "$AUR_REPO" aur-package
cd aur-package

echo "=== Updating PKGBUILD ==="

# Update _gittag, pkgver, pkgrel in PKGBUILD
sed -i "s|^_gittag=.*|_gittag=$GIT_TAG|g" PKGBUILD
sed -i "s|^pkgver=.*|pkgver=$PKGVER|g" PKGBUILD
sed -i "s|^pkgrel=.*|pkgrel=$PKGREL|g" PKGBUILD

# Download the package and calculate sha256sum
PACKAGE_URL="https://github.com/muttleyxd/arma3-unix-launcher/releases/download/$GIT_TAG/arma3-unix-launcher-$PKGVER-$PKGREL-x86_64.pkg.tar.zst"
echo "=== Downloading package from $PACKAGE_URL ==="

if ! curl -fsSL "$PACKAGE_URL" -o package.pkg.tar.zst; then
    echo "Error: Failed to download package from $PACKAGE_URL" >&2
    exit 1
fi

SHA256SUM=$(sha256sum package.pkg.tar.zst | cut -d' ' -f1)
echo "SHA256sum: $SHA256SUM"

# Update sha256sums in PKGBUILD
sed -i "s|^sha256sums=.*|sha256sums=('$SHA256SUM')|g" PKGBUILD

echo "=== Generating .SRCINFO ==="
makepkg --printsrcinfo > .SRCINFO

echo "=== Committing changes ==="
git config user.name "GitHub Actions"
git config user.email "actions@github.com"
git add PKGBUILD .SRCINFO

# Show diff of changes being committed
echo "=== Changes to be committed ==="
git diff --cached || true

git diff --cached --quiet || git commit -m "Update to $PKGVER-$PKGREL"

echo "=== Pushing to AUR ==="
git push origin master

echo "=== AUR package published successfully ==="

# Cleanup
rm -f package.pkg.tar.zst
