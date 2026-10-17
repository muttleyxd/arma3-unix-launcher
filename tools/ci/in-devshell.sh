#!/usr/bin/env bash
# Runs a command inside the flake devShell with a clean environment, so nothing from the caller
# (Homebrew, another devShell) leaks into the build. /usr/bin stays on PATH for Apple tools such
# as codesign.
#
# Usage: in-devshell.sh COMMAND [ARGS...]
#   e.g. tools/ci/in-devshell.sh cmake --build build-dev

set -u

SOURCE_DIR="$(cd "$(dirname "$0")/../.." && pwd)"

# nix-darwin's /etc/bashrc sources a zsh-only set-environment script unless this guard is set
exec nix develop --ignore-environment --keep HOME --keep TMPDIR --keep __NIX_DARWIN_SET_ENVIRONMENT_DONE \
                 "$SOURCE_DIR" --command bash -c 'export PATH="$PATH:/usr/bin:/bin"; exec "$@"' in-devshell "$@"
