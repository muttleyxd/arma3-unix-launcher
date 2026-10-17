#!/usr/bin/env bash
# Verifies that configuring with A3UL_USE_SYSTEM_DEPS=ON never downloads dependencies.
# Configure may succeed (all deps provided by the system / Nix) or fail, but a failure must
# come from the A3UL_USE_SYSTEM_DEPS guard and no FetchContent download may happen.
#
# Usage: check-offline-configure.sh [extra cmake args...]

set -u

SOURCE_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
TMP_ROOT="${TMPDIR:-/tmp}"
BUILD_DIR="$(mktemp -d "${TMP_ROOT%/}/a3ul-offline-configure.XXXXXX")" || exit 1
trap 'rm -rf "$BUILD_DIR"' EXIT

OUTPUT="$(cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" \
                -DA3UL_USE_SYSTEM_DEPS=ON \
                -DFETCHCONTENT_FULLY_DISCONNECTED=ON \
                "$@" 2>&1)"
STATUS=$?

fail()
{
    echo "$OUTPUT"
    echo
    echo "FAIL: $1"
    exit 1
}

if grep -q -- "-- Downloading" <<< "$OUTPUT"; then
    fail "configure attempted to download a dependency"
fi

if [ -n "$(ls -A "$BUILD_DIR/_deps" 2>/dev/null)" ]; then
    fail "FetchContent populated $BUILD_DIR/_deps"
fi

if [ $STATUS -ne 0 ] && ! grep -q "A3UL_USE_SYSTEM_DEPS" <<< "$OUTPUT"; then
    fail "configure failed for a reason other than the A3UL_USE_SYSTEM_DEPS guard"
fi

if [ $STATUS -eq 0 ]; then
    echo "PASS: configured with system dependencies only"
else
    echo "PASS: missing dependency reported by A3UL_USE_SYSTEM_DEPS guard, nothing downloaded"
fi
