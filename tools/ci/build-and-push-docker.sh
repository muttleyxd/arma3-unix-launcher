#!/usr/bin/env bash
set -euo pipefail

# Script to rebuild and push Arch Linux Docker image
# Reads DOCKER_USERNAME and DOCKER_PASSWORD from environment variables
# Usage: DOCKER_USERNAME=user DOCKER_PASSWORD=pass build-and-push-docker.sh

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
cd "$SCRIPT_DIR"

DOCKER_USERNAME="${DOCKER_USERNAME:?DOCKER_USERNAME environment variable required}"
DOCKER_PASSWORD="${DOCKER_PASSWORD:?DOCKER_PASSWORD environment variable required}"

echo "=== Logging in to Docker Hub ==="
# Disable command echoing to prevent password leakage in logs
set +x
echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin
set -x

echo "=== Rebuilding Arch Linux Docker image ==="
docker build --no-cache -t a3ul_archlinux_build -f docker/Dockerfile.a3ul_archlinux_build .

echo "=== Tagging image ==="
docker tag a3ul_archlinux_build "${DOCKER_USERNAME}/a3ul_archlinux_build:latest"

echo "=== Pushing to Docker Hub ==="
docker push "${DOCKER_USERNAME}/a3ul_archlinux_build:latest"

echo "=== Docker image published successfully ==="
