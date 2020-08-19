#!/bin/bash

docker build . -f android/Dockerfile -t balrogcpp/context-droid
docker create -ti --name abc balrogcpp/context-project-demo bash
mkdir artifacts
docker cp abc:/mnt/context-demo/install/ContextProjectDemo-Release-`git rev-parse -q HEAD`.zip artifacts
docker rm -f abc

exit 0
