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

echo "=== Installing runtime dependencies ==="
pacman -Sy --noconfirm fmt pugixml qt5-base qt5-svg > /dev/null 2>&1

echo "=== Fetching latest package from AUR ==="
# Try to get the latest package from AUR
TEMP_DIR=$(mktemp -d)
cd "$TEMP_DIR"

# Clone AUR package repository
if ! git clone https://aur.archlinux.org/arma3-unix-launcher-bin.git aur-pkg 2>/dev/null; then
    echo "⚠ AUR package not found, rebuild needed"
    exit 0
fi

cd aur-pkg

# Get the source URL from PKGBUILD
if ! grep -q "^source=" PKGBUILD; then
    echo "⚠ No source found in PKGBUILD, rebuild needed"
    exit 0
fi

# Extract download URL (assumes format: source=("url"))
SOURCE_URL=$(grep "^source=" PKGBUILD | sed 's/source=(//' | sed 's/)//' | tr -d '"' | tr -d "'")

if [ -z "$SOURCE_URL" ]; then
    echo "⚠ Could not parse source URL, rebuild needed"
    exit 0
fi

echo "Downloading package from: $SOURCE_URL"
if ! curl -L -o package.pkg.tar.zst "$SOURCE_URL" 2>/dev/null; then
    echo "⚠ Failed to download package, rebuild needed"
    exit 0
fi

echo "=== Extracting package ==="
if ! tar -xf package.pkg.tar.zst 2>/dev/null; then
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

# Check if ldd shows any "not found" dependencies
if echo "$LDD_OUTPUT" | grep -q "not found"; then
    echo "✗ Missing dependencies detected!"
    echo "$LDD_OUTPUT" | grep "not found"
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
