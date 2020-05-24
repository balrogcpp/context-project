#!/bin/bash

docker build . -t context &&\
docker create -ti --name abc context bash
mkdir artifacts
docker cp abc:/home/user/context-demo/artifacts/ContextProjectDemo-Release-Linux-x64.tar.gz artifacts
docker cp abc:/home/user/context-demo/artifacts/ContextProjectDemo-Release-Linux-x64.7z artifacts
docker cp abc:/home/user/context-demo/artifacts/ContextProjectDemo-Release-Windows-x64.zip artifacts
docker cp abc:/home/user/context-demo/artifacts/ContextProjectDemo-Release-Windows-x64.7z artifacts
docker cp abc:/home/user/context-demo/artifacts/ContextProjectDemo-Release-Windows-x64.exe artifacts
docker cp abc:/home/user/context-demo/artifacts/refman.pdf artifacts
docker cp abc:/home/user/context-demo/install/ContextProjectDemo-Release-`git rev-parse -q HEAD`.zip artifacts
docker rm -f abc

exit 0