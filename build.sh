#!/bin/bash

docker build . -t balrogcpp/context-project-demo
docker create -ti --name abc context bash
mkdir artifacts
docker cp abc:/mnt/context-demo/install/ContextProjectDemo-Release-`git rev-parse -q HEAD`.zip artifacts
# docker cp abc:/mnt/context-demo/artifacts/refman.pdf artifacts
# docker cp abc:/mnt/context-demo/artifacts/ContextProjectDemo-Release-Linux-x64.tar.gz artifacts
# docker cp abc:/mnt/context-demo/artifacts/ContextProjectDemo-Release-Linux-x64.7z artifacts
# docker cp abc:/mnt/context-demo/artifacts/ContextProjectDemo-Release-Windows-x64.zip artifacts
# docker cp abc:/mnt/context-demo/artifacts/ContextProjectDemo-Release-Windows-x64.7z artifacts
# docker cp abc:/mnt/context-demo/artifacts/ContextProjectDemo-Release-Windows-x64.exe artifacts
docker rm -f abc

exit 0
