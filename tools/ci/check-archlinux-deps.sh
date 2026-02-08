#!/usr/bin/env bash
set -euo pipefail

# Script to check if the latest AUR package works with current Arch Linux dependencies
# Tests by running ldd on the binary in a fresh archlinux:latest container
# Returns 0 if rebuild needed (ldd fails or deps missing), 1 if package works fine

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"

echo "=== Checking if latest AUR package works with current Arch dependencies ==="

# Create a test script that will run inside the container
TEST_SCRIPT=$(cat <<'EOF'
set -euo pipefail

echo "=== Installing git and runtime dependencies (without signature verification) ==="
# Disable signature checking to avoid keyring issues in fresh container
sed -i 's/^SigLevel.*/SigLevel = Never/' /etc/pacman.conf
# Update system first to avoid library mismatches
pacman -Syu --noconfirm > /dev/null 2>&1
pacman -S --noconfirm git curl tar fmt pugixml qt5-base qt5-svg 2>&1 | tail -10

echo "=== Fetching latest package from AUR ==="
# Try to get the latest package from AUR
TEMP_DIR=$(mktemp -d)
cd "$TEMP_DIR"

# Clone AUR package repository
echo "Cloning AUR package: arma3-unix-launcher-bin"
if ! git clone https://aur.archlinux.org/arma3-unix-launcher-bin.git aur-pkg; then
    echo "⚠ AUR package not found or git clone failed, rebuild needed"
    exit 0
fi

cd aur-pkg

# Get the source URL from PKGBUILD by sourcing it
if ! grep -q "^source=" PKGBUILD; then
    echo "⚠ No source found in PKGBUILD, rebuild needed"
    exit 0
fi

# Source the PKGBUILD to expand variables, then extract the URL
source PKGBUILD
SOURCE_URL="${source[0]}"

if [ -z "$SOURCE_URL" ]; then
    echo "⚠ Could not parse source URL, rebuild needed"
    exit 0
fi

echo "Downloading package from: $SOURCE_URL"
if ! curl -L -f -o package.pkg.tar.zst "$SOURCE_URL"; then
    echo "⚠ Failed to download package, rebuild needed"
    exit 0
fi

echo "=== Extracting package ==="
if ! tar -xf package.pkg.tar.zst; then
    echo "⚠ Failed to extract package, rebuild needed"
    exit 0
fi

if [ ! -f usr/bin/arma3-unix-launcher ]; then
    echo "⚠ Binary not found in package, rebuild needed"
    exit 0
fi

echo "=== Running ldd on binary ==="
echo ""
LDD_OUTPUT=$(ldd usr/bin/arma3-unix-launcher 2>&1 || true)
echo "$LDD_OUTPUT"
echo ""

# Check if ldd shows any "not found" dependencies (excluding bundled libs)
# libsteam_api.so is bundled with the package in usr/share/arma3-unix-launcher/lib/
# libcurlpp.so is now statically linked (as of this change)
NOT_FOUND=$(echo "$LDD_OUTPUT" | grep "not found" | grep -v "libsteam_api.so" | grep -v "libcurlpp.so" || true)
if [ -n "$NOT_FOUND" ]; then
    echo "✗ Missing dependencies detected!"
    echo "$NOT_FOUND"
    echo ""
    echo "=== Rebuild needed: AUR package is broken with current dependencies ==="
    exit 0
fi

# Check if ldd command itself failed
if echo "$LDD_OUTPUT" | grep -qi "error"; then
    echo "✗ ldd command failed!"
    echo "=== Rebuild needed: Cannot verify package integrity ==="
    exit 0
fi

echo "✓ All dependencies satisfied!"
echo "=== No rebuild needed: AUR package works with current dependencies ==="
exit 1
EOF
)

# Run the test in a fresh archlinux:latest container
if docker run --rm archlinux:latest bash -c "$TEST_SCRIPT"; then
    # Exit code 0 from script means rebuild needed
    exit 0
else
    # Exit code 1 from script means no rebuild needed
    exit 1
fi
