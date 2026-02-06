#!/usr/bin/env bash
set -euo pipefail

# Script to check Arch Linux dependency versions
# Returns 0 if dependencies changed, 1 if unchanged

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
CACHE_FILE="${1:-/tmp/arch-deps-cache.txt}"

echo "=== Checking Arch Linux dependency versions ==="

# Get current versions of key dependencies
get_current_versions() {
    docker run --rm archlinux:latest bash -c '
        pacman -Sy --noconfirm > /dev/null 2>&1
        echo "spdlog=$(pacman -Si spdlog | grep "^Version" | awk "{print \$3}")"
        echo "fmt=$(pacman -Si fmt | grep "^Version" | awk "{print \$3}")"
        echo "pugixml=$(pacman -Si pugixml | grep "^Version" | awk "{print \$3}")"
        echo "qt5-base=$(pacman -Si qt5-base | grep "^Version" | awk "{print \$3}")"
    ' 2>/dev/null | sort
}

CURRENT_VERSIONS=$(get_current_versions)
echo "Current versions:"
echo "$CURRENT_VERSIONS"

# Check if cache exists
if [ ! -f "$CACHE_FILE" ]; then
    echo "No cache found, dependencies considered changed"
    echo "$CURRENT_VERSIONS" > "$CACHE_FILE"
    exit 0
fi

# Compare with cached versions
CACHED_VERSIONS=$(cat "$CACHE_FILE")
echo ""
echo "Cached versions:"
echo "$CACHED_VERSIONS"

if [ "$CURRENT_VERSIONS" != "$CACHED_VERSIONS" ]; then
    echo ""
    echo "=== Dependencies have changed! ==="
    echo "Differences:"
    diff <(echo "$CACHED_VERSIONS") <(echo "$CURRENT_VERSIONS") || true
    echo "$CURRENT_VERSIONS" > "$CACHE_FILE"
    exit 0
else
    echo ""
    echo "=== No dependency changes detected ==="
    exit 1
fi
