#!/bin/bash

docker build . -f Docker/clang/Dockerfile -t balrogcpp/clang-cross \
&& docker build . -f dependencies/Dockerfile -t balrogcpp/xio-dependencies \
&& docker login \
&& docker push balrogcpp/clang-cross \
&& docker push balrogcpp/xio-dependencies

exit 0
