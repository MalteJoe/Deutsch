#!/usr/bin/env bash
set -euo pipefail

sudo apt-get update
sudo apt-get install -y python3.13-venv

uv tool install pebble-tool
pebble sdk install latest
