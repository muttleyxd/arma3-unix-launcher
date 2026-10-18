#!/usr/bin/env bash
# Verifies setup_steamworkssdk() handling of STEAMWORKS_SDK_DIR:
#  1. A3UL_USE_SYSTEM_DEPS=ON without STEAMWORKS_SDK_DIR fails with the guard message and downloads nothing
#  2. STEAMWORKS_SDK_DIR pointing at an SDK layout is used without downloading

set -u

SOURCE_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
TMP_ROOT="${TMPDIR:-/tmp}"
WORK_DIR="$(mktemp -d "${TMP_ROOT%/}/a3ul-steamworks-check.XXXXXX")" || exit 1
trap 'rm -rf "$WORK_DIR"' EXIT

PROJECT_DIR="$WORK_DIR/project"
FAKE_SDK_DIR="$WORK_DIR/sdk"
mkdir -p "$PROJECT_DIR" "$FAKE_SDK_DIR/include/steam" "$FAKE_SDK_DIR/lib/steam"
touch "$FAKE_SDK_DIR/include/steam/steam_api.h" "$FAKE_SDK_DIR/lib/steam/libsteam_api.dylib"

cat > "$PROJECT_DIR/CMakeLists.txt" <<EOF
cmake_minimum_required(VERSION 3.21)
project(steamworks_check CXX)
include("$SOURCE_DIR/cmake/external_dependencies.cmake")
include("$SOURCE_DIR/cmake/functions.cmake")
if (A3UL_USE_SYSTEM_DEPS)
    set(FETCHCONTENT_FULLY_DISCONNECTED ON)
endif()
setup_steamworkssdk()
get_target_property(LOCATION steamworks_sdk IMPORTED_LOCATION)
message(STATUS "STEAMWORKS_LOCATION=\${LOCATION}")
EOF

FAILED=0

run_case()
{
    local name="$1"
    shift
    rm -rf "$WORK_DIR/build"
    OUTPUT="$(cmake -S "$PROJECT_DIR" -B "$WORK_DIR/build" "$@" 2>&1)"
    STATUS=$?
    if [ -n "$(ls -A "$WORK_DIR/build/_deps" 2>/dev/null)" ]; then
        echo "$OUTPUT"
        echo "FAIL [$name]: FetchContent populated _deps"
        FAILED=1
        return 1
    fi
}

run_case "no sdk dir" -DA3UL_USE_SYSTEM_DEPS=ON
if [ $STATUS -eq 0 ] || ! grep -q "A3UL_USE_SYSTEM_DEPS" <<< "$OUTPUT"; then
    echo "$OUTPUT"
    echo "FAIL [no sdk dir]: expected configure to fail with the A3UL_USE_SYSTEM_DEPS guard"
    FAILED=1
else
    echo "PASS [no sdk dir]"
fi

run_case "sdk dir" -DA3UL_USE_SYSTEM_DEPS=ON "-DSTEAMWORKS_SDK_DIR=$FAKE_SDK_DIR"
if [ $STATUS -ne 0 ] || ! grep -q "STEAMWORKS_LOCATION=$FAKE_SDK_DIR/lib/steam/libsteam_api.dylib" <<< "$OUTPUT"; then
    echo "$OUTPUT"
    echo "FAIL [sdk dir]: expected steamworks_sdk to point at $FAKE_SDK_DIR/lib/steam/libsteam_api.dylib"
    FAILED=1
else
    echo "PASS [sdk dir]"
fi

exit $FAILED
