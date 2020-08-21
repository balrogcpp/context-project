#!/bin/bash

docker build . -t balrogcpp/context-project-demo
docker create -ti --name abc balrogcpp/context-project-demo bash
mkdir artifacts
docker cp abc:/mnt/context-demo/install/ContextProjectDemo-Release-`git rev-parse -q HEAD`.zip artifacts
# docker cp abc:/mnt/context-demo/artifacts/refman.pdf artifacts
docker rm -f abc

exit 0
