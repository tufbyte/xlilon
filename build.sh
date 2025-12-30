#!/bin/bash

# ==========================================
# Xlilon Build Script (Linux / macOS)
# ==========================================

# 1. Create the build directory if it doesn't exist
mkdir -p build

echo "[BUILD] Compiling Xlilon..."

# 2. Compile!
# -----------------------------------------------------------------------
# Compiler Flags:
#  -g           : Generate debug information (for gdb/lldb).
#  -Wall        : Enable all standard warnings.
#  -std=c++17   : Use C++17 standard.
#
# Source Files:
#  src/platform/linux_runner.cpp : The OS entry point.
#  src/xlilon.cpp                : The library implementation.
#
# Output:
#  -o build/xlilon : Places the binary inside the build folder.
#
# Libraries (-l):
#  -lX11        : Links against the X11 library (Required for Linux).
# -----------------------------------------------------------------------

# Detect OS to choose the correct runner and flags
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # LINUX BUILD
    g++ -g -Wall -std=c++17 \
        src/platform/linux_runner.cpp \
        src/xlilon.cpp \
        -o build/xlilon \
        -lX11

elif [[ "$OSTYPE" == "darwin"* ]]; then
    # MACOS BUILD (Requires Objective-C++ for Cocoa interaction)
    # We use clang++ because it handles .mm files (Obj-C++) natively.
    clang++ -g -Wall -std=c++17 \
        src/platform/macos_runner.mm \
        src/xlilon.cpp \
        -o build/xlilon \
        -framework Cocoa

else
    echo "Unknown OS: $OSTYPE"
    exit 1
fi

# Check if compilation succeeded
if [ $? -eq 0 ]; then
    echo "[BUILD] Finished successfully."
    echo "Run with: ./build/xlilon"
else
    echo "[BUILD] Failed."
fi
