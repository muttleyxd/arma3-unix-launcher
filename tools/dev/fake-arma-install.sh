#!/usr/bin/env bash
# Creates a fake Arma 3 install with many Workshop mods and a separate launcher config pointing at
# it, for trying the launcher with a large mod list without touching the real config.
# The first two mods use the real ACE3 and CBA_A3 Workshop ids, so their thumbnails load; the
# others get placeholders.
#
# Usage: fake-arma-install.sh [MOD_COUNT (default 500)] [DIRECTORY (default: a new temp dir)]

set -eu

MOD_COUNT="${1:-500}"
ROOT="${2:-$(mktemp -d "${TMPDIR:-/tmp}/a3ul-fake-arma.XXXXXX")}"

ARMA="$ROOT/arma3"
WORKSHOP="$ROOT/workshop"
CONFIG_HOME="$ROOT/config"

mkdir -p "$ARMA/ArmA3.app" "$WORKSHOP" "$CONFIG_HOME/a3unixlauncher"

real_mods=("463939057:ace" "450814997:CBA_A3")
for i in $(seq 1 "$MOD_COUNT"); do
    if [ "$i" -le "${#real_mods[@]}" ]; then
        id="${real_mods[$((i - 1))]%%:*}"
        name="${real_mods[$((i - 1))]#*:}"
    else
        id=$((900000000 + i))
        case $((i % 4)) in
            0) kind="Medical" ;;
            1) kind="Vehicles" ;;
            2) kind="Weapons" ;;
            *) kind="Terrain" ;;
        esac
        name="Fake $kind Mod $i"
    fi
    mkdir -p "$WORKSHOP/$id"
    printf 'protocol = 1;\npublishedid = %s;\nname = "%s";\n' "$id" "$name" > "$WORKSHOP/$id/meta.cpp"
done

cat > "$CONFIG_HOME/a3unixlauncher/launcher.conf" <<EOF
{
    "paths": { "arma": "$ARMA", "workshop": "$WORKSHOP" },
    "settings": { "checkForUpdates": false }
}
EOF

echo "Created $MOD_COUNT Workshop mods in $ROOT"
echo "Run the launcher against it (Steam integration off) with:"
echo "  XDG_CONFIG_HOME='$CONFIG_HOME' nix run . -- --disable-steam-integration"
