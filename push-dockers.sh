#!/bin/bash


docker build . -f docker/clang-linux/Dockerfile -t balrogcpp/clang-linux \
&& docker build . -f docker/deps-linux/Dockerfile -t balrogcpp/xio-deps-linux \
&& docker build . -f docker/clang-cross/Dockerfile -t balrogcpp/clang-cross \
&& docker build . -f docker/deps-cross/Dockerfile -t balrogcpp/xio-deps-cross \
&& docker build . -f docker/clang-android/Dockerfile -t balrogcpp/clang-android \
&& docker build . -f docker/deps-android/Dockerfile -t balrogcpp/xio-deps-android \
&& docker login \
&& docker push balrogcpp/clang-linux \
&& docker push balrogcpp/clang-cross \
&& docker push balrogcpp/clang-android \
&& docker push balrogcpp/xio-deps-linux \
&& docker push balrogcpp/xio-deps-cross \
&& docker push balrogcpp/xio-deps-android


exit 0
