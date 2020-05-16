#!/bin/bash

docker build . -t context &&\
docker create -ti --name abc context bash
mkdir artifacts
docker cp abc:/home/user/context/artifacts/ContextProjectDemo-Release-Linux-x64.tar.gz artifacts
docker cp abc:/home/user/context/artifacts/ContextProjectDemo-Release-Linux-x64.7z artifacts
docker cp abc:/home/user/context/artifacts/ContextProjectDemo-Release-Windows-x64.zip artifacts
docker cp abc:/home/user/context/artifacts/ContextProjectDemo-Release-Windows-x64.7z artifacts
docker cp abc:/home/user/context/artifacts/ContextProjectDemo-Release-Windows-x64.exe artifacts
docker cp abc:/home/user/context/artifacts/refman.pdf artifacts
docker rm -f abc

exit 0