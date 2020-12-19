#!/bin/bash

docker build . -f Docker/gcc/Dockerfile -t balrogcpp/gcc-mingw-x86_64 &&\
docker build . -f dependencies/Dockerfile -t balrogcpp/xio-dependencies &&\
docker login &&\
docker push balrogcpp/gcc-mingw-x86_64 &&\
docker push balrogcpp/xio-dependencies

exit 0
