#!/bin/bash

set -e

# Detect ISA
ARCH=$(uname -m)
echo "Detected architecture: $ARCH"

# Detect OS
OS=$(uname -s)
echo "Detected operating system: $OS"

install_mac() {
    echo "Using Homebrew on macOS"
    if ! command -v brew >/dev/null; then
        echo "Homebrew not found. Install it from https://brew.sh"
        exit 1
    fi
    brew install cpr nlohmann-json
}

install_apt() {
    echo "Using APT on Debian/Ubuntu"
    sudo apt update
    sudo apt install -y libcpr-dev nlohmann-json-dev
}

install_dnf() {
    echo "Using DNF on Fedora"
    sudo dnf install -y cpr-devel nlohmann-json-devel
}

install_pacman() {
    echo "Using pacman on Arch"
    sudo pacman -Sy --noconfirm cpr nlohmann-json
}

install_choco() {
    echo "Using Chocolatey on Windows"
    if ! command -v choco >/dev/null; then
        echo "Chocolatey not found. Please install it: https://chocolatey.org/install"
        exit 1
    fi
    choco install -y cpr nlohmann.json
}

# Dispatch based on OS
case "$OS" in
    Darwin)
        install_mac
        ;;
    Linux)
        if command -v apt >/dev/null; then
            install_apt
        elif command -v dnf >/dev/null; then
            install_dnf
        elif command -v pacman >/dev/null; then
            install_pacman
        else
            echo "Unsupported Linux distribution. Install cpr and nlohmann-json manually."
            exit 1
        fi
        ;;
    MINGW*|MSYS*|CYGWIN*)
        install_choco
        ;;
    *)
        echo "Unsupported or unknown OS: $OS"
        exit 1
        ;;
esac

echo "✅ Installation complete for $OS ($ARCH)"
