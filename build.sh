#!/bin/bash

docker build --build-arg GIT_HASH=`git rev-parse --short=8 HEAD`  . -f Docker/xio-linux/Dockerfile -t balrogcpp/xio-linux
docker build --build-arg GIT_HASH=`git rev-parse --short=8 HEAD`  . -f Docker/xio-cross/Dockerfile -t balrogcpp/xio-cross
docker create -ti --name abc balrogcpp/xio-linux bash
mkdir artifacts
docker cp abc:/mnt/build/artifacts/ContextProjectDemo-linux-clang-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts
docker rm -f abc
docker create -ti --name abc balrogcpp/xio-cross bash
docker cp abc:/mnt/build/artifacts/ContextProjectDemo-windows-clang-mingw-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts
docker cp abc:/mnt/build/artifacts/ContextProjectDemo-apple-darwin-clang-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts
docker rm -f abc

exit 0
