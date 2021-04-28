#!/bin/bash

mkdir artifacts


docker build --build-arg GIT_HASH=`git rev-parse --short=8 HEAD`  . -f docker/xio-linux/Dockerfile -t balrogcpp/xio-linux \
&& docker create -ti --name abc balrogcpp/xio-linux bash \
&& docker cp abc:/mnt/build/artifacts/ContextProjectDemo-linux-clang-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts

docker rm -f abc



docker build --build-arg GIT_HASH=`git rev-parse --short=8 HEAD`  . -f docker/xio-cross/Dockerfile -t balrogcpp/xio-cross \
&& docker create -ti --name abc balrogcpp/xio-cross bash \
&& docker cp abc:/mnt/build/artifacts/ContextProjectDemo-windows-clang-mingw-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts \
&& docker cp abc:/mnt/build/artifacts/ContextProjectDemo-apple-darwin-clang-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts

docker rm -f abc



docker build --build-arg GIT_HASH=`git rev-parse --short=8 HEAD`  . -f docker/xio-android/Dockerfile -t balrogcpp/xio-android \
&& docker create -ti --name abc balrogcpp/xio-android bash \
&& docker cp abc:/mnt/build/artifacts/app-arm64-v8a-release.apk artifacts \
&& docker cp abc:/mnt/build/artifacts/app-x86_64-release.apk  artifacts

docker rm -f abc


exit 0
