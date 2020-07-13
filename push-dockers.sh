#!/bin/bash

docker build . -f gcc/Dockerfile -t balrogcpp/mingw-gcc10-cmake &&\
docker build . -f thirdparty/Dockerfile -t balrogcpp/context-project-dependencies &&\
docker login &&\
docker push balrogcpp/mingw-gcc10-cmake &&\
docker push balrogcpp/context-project-dependencies

exit 0