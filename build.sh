#!/bin/bash

docker build --build-arg GIT_HASH=`git rev-parse --short=8 HEAD` . -t balrogcpp/context-project-demo
docker create -ti --name abc balrogcpp/context-project-demo bash
mkdir artifacts
docker cp abc:/mnt/build/artifacts/ContextProjectDemo-Linux-x64-`git rev-parse --short=8 HEAD`.zip artifacts
docker rm -f abc

exit 0
