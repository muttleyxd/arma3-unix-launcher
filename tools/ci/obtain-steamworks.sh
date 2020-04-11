#!/usr/bin/env bash
if [ -z $GPG_DECRYPTION_KEY_BASE64 ]; then
    echo "This script is intended for CI usage by arma3-unix-launcher, you shouldn't run it"
    exit 1
fi

set -euo pipefail

mkdir /tmp/steamworks && cd /tmp/steamworks >/dev/null 2>&1
wget https://github.com/muttleyxd/gpg_encrypted_secrets/releases/download/secrets/steamworks.7z.gpg >/dev/null 2>&1
echo $GPG_DECRYPTION_KEY_BASE64 | base64 -d >/tmp/key.gpg >/dev/null 2>&1
gpg --import /tmp/key.gpg >/dev/null 2>&1
gpg --output steamworks.7z --decrypt steamworks.7z.gpg >/dev/null 2>&1
7z x steamworks.7z >/dev/null 2>&1
