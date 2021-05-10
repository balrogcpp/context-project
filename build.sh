#!/bin/bash

mkdir artifacts


docker build --build-arg GIT_HASH=`git rev-parse --short=8 HEAD`  . -f docker/build-linux/Dockerfile -t balrogcpp/build-linux \
&& docker create -ti --name abc balrogcpp/build-linux bash \
&& docker cp abc:/mnt/build/artifacts/ContextProjectDemo-linux-clang-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts

docker rm -f abc



docker build --build-arg GIT_HASH=`git rev-parse --short=8 HEAD`  . -f docker/build-cross/Dockerfile -t balrogcpp/build-cross \
&& docker create -ti --name abc balrogcpp/build-cross bash \
&& docker cp abc:/mnt/build/artifacts/ContextProjectDemo-windows-clang-mingw-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts \
&& docker cp abc:/mnt/build/artifacts/ContextProjectDemo-apple-darwin-clang-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts \
&& docker cp abc:/mnt/build/artifacts/app-arm64-v8a-`git rev-parse --short=8 HEAD`.apk artifacts

docker rm -f abc


exit 0
