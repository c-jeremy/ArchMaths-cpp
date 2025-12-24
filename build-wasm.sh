#!/bin/bash
set -e

# ArchMaths WebAssembly Build Script
# Prerequisites:
#   1. Emscripten SDK: https://emscripten.org/docs/getting_started/downloads.html
#   2. Qt6 for WebAssembly: pip install aqtinstall && aqt install-qt linux desktop 6.6.0 wasm_singlethread -m qtbase

# Configuration - adjust these paths
EMSDK_PATH="${EMSDK_PATH:-$HOME/emsdk}"
QT_WASM_PATH="${QT_WASM_PATH:-$HOME/Qt/6.6.0/wasm_singlethread}"

# Validate paths
if [ ! -f "$EMSDK_PATH/emsdk_env.sh" ]; then
    echo "Error: Emscripten SDK not found at $EMSDK_PATH"
    echo "Set EMSDK_PATH environment variable or install from:"
    echo "  git clone https://github.com/emscripten-core/emsdk.git"
    echo "  cd emsdk && ./emsdk install latest && ./emsdk activate latest"
    exit 1
fi

if [ ! -d "$QT_WASM_PATH" ]; then
    echo "Error: Qt6 WebAssembly not found at $QT_WASM_PATH"
    echo "Set QT_WASM_PATH environment variable or install with:"
    echo "  pip install aqtinstall"
    echo "  aqt install-qt linux desktop 6.6.0 wasm_singlethread -m qtbase"
    exit 1
fi

# Source Emscripten environment
source "$EMSDK_PATH/emsdk_env.sh"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Build
echo "Building ArchMaths for WebAssembly..."
mkdir -p "$SCRIPT_DIR/build-wasm"
cd "$SCRIPT_DIR/build-wasm"

emcmake cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DQt6_DIR="$QT_WASM_PATH/lib/cmake/Qt6"

emmake make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Build complete! Output files:"
ls -lh ArchMaths.html ArchMaths.js ArchMaths.wasm 2>/dev/null || ls -lh *.html *.js *.wasm 2>/dev/null

echo ""
echo "To deploy, copy these files to your web server:"
echo "  - ArchMaths.html (rename to index.html)"
echo "  - ArchMaths.js"
echo "  - ArchMaths.wasm"
echo "  - ArchMaths.data (if present)"
echo ""
echo "Web server must include these headers:"
echo "  Cross-Origin-Opener-Policy: same-origin"
echo "  Cross-Origin-Embedder-Policy: require-corp"
