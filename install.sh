#!/usr/bin/env bash
set -e

if ! command -v make >/dev/null 2>&1; then
    echo "Error: make is not installed."
    echo "Try: sudo apt make install or sudo pacman -S make"
    exit 1
fi

touch config.h
cat config.def.h > config.h

make
sudo make install

echo "♥︎ Installed sysbloom to /usr/local/bin/sysbloom"
