#!/bin/bash

docker build --build-arg GIT_HASH=`git rev-parse --short=8 HEAD` . -t balrogcpp/context-project-demo
docker create -ti --name abc balrogcpp/context-project-demo bash
mkdir artifacts
docker cp abc:/mnt/build/artifacts/ContextProjectDemo-linux-clang-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts
docker cp abc:/mnt/build/artifacts/ContextProjectDemo-windows-clang-mingw-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts
docker cp abc:/mnt/build/artifacts/ContextProjectDemo-apple-darwin-clang-x86_64-`git rev-parse --short=8 HEAD`.zip artifacts
docker rm -f abc

exit 0
