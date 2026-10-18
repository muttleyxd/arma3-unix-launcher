#!/usr/bin/env bash
# Verifies the flake devShell alone is enough to configure (offline), build and test the launcher.
# Runs inside an isolated `nix develop` so nothing from the caller's environment (Homebrew,
# another devShell) leaks into the build.
#
# Usage: check-devshell.sh [build dir, default: build-devshell-check]

set -u

SOURCE_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${1:-build-devshell-check}"

cd "$SOURCE_DIR" || exit 1

# nix-darwin's /etc/bashrc sources a zsh-only set-environment script unless this guard is set
nix develop --ignore-environment --keep HOME --keep TMPDIR --keep __NIX_DARWIN_SET_ENVIRONMENT_DONE \
            "$SOURCE_DIR" --command bash -c '
    set -u
    BUILD_DIR="$1"
    if [ -z "${STEAMWORKS_SDK_DIR:-}" ]; then
        echo "FAIL: devShell does not export STEAMWORKS_SDK_DIR"
        exit 1
    fi
    tools/ci/check-offline-configure.sh --require-success -DRUN_TESTS=ON || exit 1
    cmake -S . -B "$BUILD_DIR" -DRUN_TESTS=ON -DDEVELOPER_MODE=ON -DCMAKE_BUILD_TYPE=Debug \
          -DCMAKE_CXX_FLAGS=-Werror || { echo "FAIL: configure"; exit 1; }
    cmake --build "$BUILD_DIR" --parallel || { echo "FAIL: build"; exit 1; }
    ctest --test-dir "$BUILD_DIR" --output-on-failure || { echo "FAIL: tests"; exit 1; }
    echo "PASS: devShell configures offline, builds and passes tests"
' check-devshell "$BUILD_DIR"
