#!/bin/bash

mkdir Artifacts
docker build -t cross_deps . -f cross_deps.Dockerfile &&\
docker run -it -d cross_deps &&\
CONTAINER_ID=$(docker ps -alq)
docker cp $CONTAINER_ID:/mnt/Source/Dependencies/External/Linux_x86_64_Clang_Release.tar.xz ./Artifacts/Linux_x86_64_Clang_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/Source/Dependencies/External/Windows_x86_64_Clang_Mingw_Release.tar.xz ./Artifacts/Windows_x86_64_Clang_Mingw_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/Source/Dependencies/External/Darwin_x86_64_Clang_Release.tar.xz ./Artifacts/Darwin_x86_64_Clang_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/Source/Dependencies/External/Emscripten_x86_Clang_Release.tar.xz ./Artifacts/Emscripten_x86_Clang_Release.tar.xz
docker cp $CONTAINER_ID:/mnt/Source/Dependencies/External/Android_aarch64_Clang_Release.tar.xz ./Artifacts/Android_aarch64_Clang_Release.tar.xz

exit 0
