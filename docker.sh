#!/bin/bash

mkdir Artifacts
docker build -t cross_test . &&\
docker run -it -d cross_test &&\
CONTAINER_ID=$(docker ps -alq)
docker cp $CONTAINER_ID:/mnt/Artifacts/GlueSample_Linux_x86_64.tar.xz ./Artifacts/GlueSample_Linux_x86_64.tar.xz
docker cp $CONTAINER_ID:/mnt/Artifacts/GlueSample_Windows_x86_64.zip ./Artifacts/GlueSample_Windows_x86_64.zip
docker cp $CONTAINER_ID:/mnt/Artifacts/GlueSample_Darwin_x86_64.tar.xz ./Artifacts/GlueSample_Darwin_x86_64.tar.xz
docker cp $CONTAINER_ID:/mnt/Artifacts/GlueSample_Emscripten_x86.tar.xz ./Artifacts/GlueSample_Emscripten_x86.tar.xz
docker cp $CONTAINER_ID:/mnt/Source/Engine/android/build/outputs/apk/release/android-arm64-v8a-release.apk ./Artifacts/GlueSample_Android_aarch64.apk
docker stop $CONTAINER_ID

exit 0
