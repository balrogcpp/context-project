#!/bin/bash

docker build . -f Docker/clang-linux/Dockerfile -t balrogcpp/clang-linux \
&& docker build . -f Docker/clang-cross/Dockerfile -t balrogcpp/clang-cross \
&& docker build . -f Docker/deps-linux/Dockerfile -t balrogcpp/xio-deps-linux \
&& docker build . -f Docker/deps-cross/Dockerfile -t balrogcpp/xio-deps-cross \
&& docker login \
&& docker push balrogcpp/clang-linux \
&& docker push balrogcpp/clang-cross \
&& docker push balrogcpp/xio-deps-linux \
&& docker push balrogcpp/xio-deps-cross

exit 0
