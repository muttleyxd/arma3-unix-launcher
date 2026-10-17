#!/usr/bin/env bash
# Verifies the flake package and checks:
#  1. `nix build` produces "Arma 3 Unix Launcher.app" with libsteam_api.dylib bundled
#  2. the app runs outside any Nix shell (`--help` succeeds with a clean environment)
#  3. the bundle is self-contained: no Mach-O file in it links against /nix/store, and the
#     package closure is the app alone
#  4. every Mach-O file in the bundle carries an embedded (ad-hoc) signature
#  5. `nix flake check` passes
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

DEPLOY_ERRORS="$(nix log "$OUT" 2>/dev/null | grep -E "^ERROR: ")"
if [ -n "$DEPLOY_ERRORS" ]; then
    echo "$DEPLOY_ERRORS" | sort | uniq -c
    fail "macdeployqt reported errors during the build"
fi
echo "PASS: macdeployqt ran without errors"

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
UNSIGNED=""
while IFS= read -r -d '' file; do
    if file -b "$file" | grep -q "Mach-O"; then
        refs="$(otool -L "$file" | tail -n +2 | grep "/nix/store/")"
        if [ -n "$refs" ]; then
            STORE_REFERENCES+="${file#"$APP/"}:"$'\n'"$refs"$'\n'
        fi
        # Ad-hoc signatures from the sandbox cannot seal bundle resources, so only require an
        # embedded signature here; the .dmg app re-signs the whole bundle with Apple's codesign
        if ! /usr/bin/codesign --display "$file" > /dev/null 2>&1; then
            UNSIGNED+="${file#"$APP/"}"$'\n'
        fi
    fi
done < <(find "$APP" -type f -print0)

if [ -n "$UNSIGNED" ]; then
    echo "$UNSIGNED"
    fail "Mach-O files without an embedded signature (arm64 refuses to load them)"
fi
echo "PASS: all Mach-O files are signed"

if [ -n "$STORE_REFERENCES" ]; then
    echo "$STORE_REFERENCES"
    fail "bundle links against /nix/store, it would not run on another Mac"
fi
echo "PASS: bundle is self-contained"

CLOSURE="$(nix path-info --recursive "$OUT" | grep -vxF "$OUT")"
if [ -n "$CLOSURE" ]; then
    echo "$CLOSURE"
    fail "package still references other store paths (embedded path strings), closure is not minimal"
fi
echo "PASS: package closure contains only the app"

if [ $SKIP_FLAKE_CHECK -eq 0 ]; then
    nix flake check "$SOURCE_DIR" || fail "nix flake check"
    echo "PASS: nix flake check"
fi
