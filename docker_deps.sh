#!/bin/bash

mkdir artifacts
docker build -t cross_deps . -f cross_deps.Dockerfile &&\
docker run -it -d cross_deps &&\
CONTAINER_ID=$(docker ps -alq)
docker cp $CONTAINER_ID:/mnt/contrib/Linux_x86_64_Clang_Release.tar.xz ./artifacts/Linux_x86_64_Clang_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/contrib/Windows_x86_64_Clang_Mingw_Release.tar.xz ./artifacts/Windows_x86_64_Clang_Mingw_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/contrib/Darwin_x86_64_Clang_Release.tar.xz ./artifacts/Darwin_x86_64_Clang_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/contrib/Emscripten_x86_Clang_Release.tar.xz ./artifacts/Emscripten_x86_Clang_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/contrib/Android_aarch64_Clang_Release.tar.xz ./artifacts/Android_aarch64_Clang_Release.tar.xz
docker stop $CONTAINER_ID
