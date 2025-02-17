#!/bin/bash

arch=$(uname -m)
export ARCH=$arch
echo "Building for $arch"
cmake -B build
cmake --build build -j$(nproc)
