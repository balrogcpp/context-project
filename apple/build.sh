#!/bin/bash

eval `x86_64-apple-darwin19-osxcross-conf`
export OSXCROSS_HOST=x86_64-apple-darwin19
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -G Ninja ..
cmake --build . --target target-deps
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -G Ninja ..
cmake --build . --target demo

exit 0
