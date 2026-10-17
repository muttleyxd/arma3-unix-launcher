#!/usr/bin/env bash
# Verifies the flake package and checks:
#  1. `nix build` produces "Arma 3 Unix Launcher.app" with libsteam_api.dylib bundled
#  2. the app runs outside any Nix shell (`--help` succeeds with a clean environment)
#  3. the bundle is self-contained: no Mach-O file in it links against /nix/store
#  4. `nix flake check` passes
#
# Usage: check-nix-package.sh [--skip-flake-check]

set -u

SKIP_FLAKE_CHECK=0
if [ "${1:-}" = "--skip-flake-check" ]; then
    SKIP_FLAKE_CHECK=1
fi

SOURCE_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$SOURCE_DIR" || exit 1

fail()
{
    echo "FAIL: $1"
    exit 1
}

OUT="$(nix build --no-link --print-out-paths "$SOURCE_DIR#default")" || fail "nix build"
APP="$OUT/Applications/Arma 3 Unix Launcher.app"
BINARY="$APP/Contents/MacOS/Arma 3 Unix Launcher"

[ -x "$BINARY" ] || fail "missing $BINARY"
[ -f "$APP/Contents/MacOS/libsteam_api.dylib" ] || fail "libsteam_api.dylib not bundled"
echo "PASS: nix build produced $APP"

HELP_OUTPUT="$(env -i HOME="$HOME" PATH=/usr/bin:/bin "$BINARY" --help 2>&1)" || {
    echo "$HELP_OUTPUT"
    fail "app --help failed outside the Nix shell"
}
grep -q -- "--preset-to-run" <<< "$HELP_OUTPUT" || {
    echo "$HELP_OUTPUT"
    fail "unexpected --help output"
}
echo "PASS: app runs outside the Nix shell"

STORE_REFERENCES=""
while IFS= read -r -d '' file; do
    if file -b "$file" | grep -q "Mach-O"; then
        refs="$(otool -L "$file" | tail -n +2 | grep "/nix/store/")"
        if [ -n "$refs" ]; then
            STORE_REFERENCES+="${file#"$APP/"}:"$'\n'"$refs"$'\n'
        fi
    fi
done < <(find "$APP" -type f -print0)

if [ -n "$STORE_REFERENCES" ]; then
    echo "$STORE_REFERENCES"
    fail "bundle links against /nix/store, it would not run on another Mac"
fi
echo "PASS: bundle is self-contained"

if [ $SKIP_FLAKE_CHECK -eq 0 ]; then
    nix flake check "$SOURCE_DIR" || fail "nix flake check"
    echo "PASS: nix flake check"
fi
