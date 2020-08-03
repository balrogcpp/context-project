#!/bin/bash

docker build . -f gcc/Dockerfile -t balrogcpp/gcc-mingw-x86_64 &&\
docker build . -f thirdparty/Dockerfile -t balrogcpp/context-project-dependencies &&\
docker login &&\
docker push balrogcpp/gcc-mingw-x86_64 &&\
docker push balrogcpp/context-project-dependencies

exit 0