#!/bin/bash

docker build . -t context &&\
docker create -ti --name abc context bash
mkdir artifacts
docker cp abc:/home/user/context/artifacts/ContextProjectDemo-Release-x64.tar.gz artifacts
docker cp abc:/home/user/context/artifacts/ContextProjectDemo-Release-x64.zip artifacts
docker rm -f abc

exit 0