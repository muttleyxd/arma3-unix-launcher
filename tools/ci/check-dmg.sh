#!/usr/bin/env bash
# Verifies `nix run .#dmg` produces a distributable disk image:
#  1. the .dmg is created and passes `hdiutil verify`
#  2. it contains "Arma 3 Unix Launcher.app" and an Applications symlink
#  3. the app passes Apple's strict deep signature verification (sealed bundle)
#  4. the app runs from the mounted image

set -u

SOURCE_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
TMP_ROOT="${TMPDIR:-/tmp}"
WORK_DIR="$(mktemp -d "${TMP_ROOT%/}/a3ul-dmg-check.XXXXXX")" || exit 1
MOUNT_POINT="$WORK_DIR/mount"
DMG="$WORK_DIR/Arma 3 Unix Launcher.dmg"

cleanup()
{
    if [ -d "$MOUNT_POINT" ]; then
        hdiutil detach "$MOUNT_POINT" -quiet 2>/dev/null
    fi
    rm -rf "$WORK_DIR"
}
trap cleanup EXIT

fail()
{
    echo "FAIL: $1"
    exit 1
}

nix run "$SOURCE_DIR#dmg" -- "$DMG" || fail "nix run .#dmg"
[ -f "$DMG" ] || fail "no disk image at $DMG"
hdiutil verify "$DMG" -quiet || fail "hdiutil verify"
echo "PASS: disk image created and verified"

mkdir -p "$MOUNT_POINT"
hdiutil attach "$DMG" -readonly -nobrowse -noautoopen -mountpoint "$MOUNT_POINT" -quiet || fail "hdiutil attach"

APP="$MOUNT_POINT/Arma 3 Unix Launcher.app"
[ -d "$APP" ] || fail "image does not contain Arma 3 Unix Launcher.app"
[ "$(readlink "$MOUNT_POINT/Applications")" = "/Applications" ] || fail "image has no Applications symlink"
echo "PASS: image contains the app and an Applications link"

/usr/bin/codesign --verify --deep --strict "$APP" || fail "app signature does not verify"
echo "PASS: app bundle signature verifies"

HELP_OUTPUT="$(env -i HOME="$HOME" PATH=/usr/bin:/bin "$APP/Contents/MacOS/Arma 3 Unix Launcher" --help 2>&1)" || {
    echo "$HELP_OUTPUT"
    fail "app from the image does not run"
}
grep -q -- "--preset-to-run" <<< "$HELP_OUTPUT" || fail "unexpected --help output"
echo "PASS: app runs from the image"
