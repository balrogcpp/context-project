#!/bin/bash

mkdir Artifacts
docker build -t cross_deps . -f cross_deps.Dockerfile &&\
docker run -it -d cross_deps &&\
CONTAINER_ID=$(docker ps -alq)
docker cp $CONTAINER_ID:/mnt/Dependencies/Linux_x86_64_Clang_Release.tar.xz ./Artifacts/Linux_x86_64_Clang_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/Dependencies/Windows_x86_64_Clang_Mingw_Release.tar.xz ./Artifacts/Windows_x86_64_Clang_Mingw_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/Dependencies/Darwin_x86_64_Clang_Release.tar.xz ./Artifacts/Darwin_x86_64_Clang_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/Dependencies/Emscripten_x86_Clang_Release.tar.xz ./Artifacts/Emscripten_x86_Clang_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/Dependencies/Android_aarch64_Clang_Release.tar.xz ./Artifacts/Android_aarch64_Clang_Release.tar.xz
docker stop $CONTAINER_ID

exit 0
