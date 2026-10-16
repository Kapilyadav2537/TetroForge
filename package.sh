#!/usr/bin/env bash
# Packages ONLY files necessary to build and run TetroForge with README.md (no runtime files/folders)
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

ZIP_NAME="tetroforge.zip"
rm -f "${ZIP_NAME}"

echo "Creating ${ZIP_NAME} with only files necessary to run the game and README.md..."
zip -r "${ZIP_NAME}" CMakeLists.txt CMakePresets.json README.md cmake/ProjectOptions.cmake src/ -x "*.o" "*.a" "*.so"

echo "Done! Package created: ${ZIP_NAME}"
