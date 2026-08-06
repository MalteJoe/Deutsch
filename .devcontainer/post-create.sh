#!/usr/bin/env bash
set -euo pipefail

sudo apt-get update
sudo apt-get install -y gcc-arm-none-eabi

python3 -m pip install --user --upgrade pip
python3 -m pip install --user pebble-tool

export PATH="$HOME/.local/bin:$PATH"

if ! command -v pebble >/dev/null 2>&1; then
  echo "Pebble CLI is not available on PATH after installation." >&2
  exit 1
fi

pebble sdk install latest || true
