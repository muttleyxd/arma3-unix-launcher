#!/usr/bin/env bash
set -euo pipefail

# Script to setup SSH configuration for AUR publishing
# Reads AUR_SSH_PRIVATE_KEY from environment variable
# Usage: AUR_SSH_PRIVATE_KEY="key" setup-aur-ssh.sh <output_dir>

OUTPUT_DIR="${1:-.ssh-aur}"
AUR_SSH_PRIVATE_KEY="${AUR_SSH_PRIVATE_KEY:?AUR_SSH_PRIVATE_KEY environment variable required}"

echo "=== Setting up SSH for AUR ==="

mkdir -p "$OUTPUT_DIR"

# Write SSH private key
set +x
echo "$AUR_SSH_PRIVATE_KEY" > "$OUTPUT_DIR/aur"
chmod 600 "$OUTPUT_DIR/aur"
set -x

# Get AUR host key
ssh-keyscan aur.archlinux.org > "$OUTPUT_DIR/known_hosts"

# Create SSH config
cat > "$OUTPUT_DIR/config" << 'EOF'
Host aur.archlinux.org
  IdentityFile /root/.ssh/aur
  User aur
  StrictHostKeyChecking no
EOF
chmod 600 "$OUTPUT_DIR/config"

echo "=== SSH configuration ready at $OUTPUT_DIR ==="
