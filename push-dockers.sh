#!/bin/bash

docker build . -f Docker/clang/Dockerfile -t balrogcpp/clang-mingw-x86_64 \
&& docker build . -f dependencies/Dockerfile -t balrogcpp/xio-dependencies \
&& docker login \
&& docker push balrogcpp/clang-mingw-x86_64 \
&& docker push balrogcpp/xio-dependencies

exit 0
