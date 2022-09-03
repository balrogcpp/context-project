#!/bin/bash

mkdir artifacts
docker build -t cross_test . &&\
docker run -it -d cross_test &&\
CONTAINER_ID=$(docker ps -alq)
GIT_SHA1=$(git rev-parse --short=8 HEAD)
docker cp $CONTAINER_ID:/mnt/artifacts/GlueSample_Linux_x86_64.tar.xz ./artifacts/GlueSample_Linux_x86_64_$GIT_SHA1.tar.xz
docker cp $CONTAINER_ID:/mnt/artifacts/GlueSample_Windows_x86_64.zip ./artifacts/GlueSample_Windows_x86_64_$GIT_SHA1.zip
docker cp $CONTAINER_ID:/mnt/artifacts/GlueSample_Darwin_x86_64.tar.xz ./artifacts/GlueSample_Darwin_x86_64_$GIT_SHA1.tar.xz
docker cp $CONTAINER_ID:/mnt/artifacts/GlueSample_Emscripten_x86.tar.xz ./artifacts/GlueSample_Emscripten_x86_$GIT_SHA1.tar.xz
docker cp $CONTAINER_ID:/mnt/Source/Engine/android/build/outputs/apk/release/android-arm64-v8a-release.apk ./artifacts/GlueSample_Android_aarch64_$GIT_SHA1.apk
docker stop $CONTAINER_ID
