#!/bin/bash


docker build . -f docker/clang-linux/Dockerfile -t balrogcpp/clang-linux \
&& docker build . -f docker/clang-cross/Dockerfile -t balrogcpp/clang-cross \
&& docker login \
&& docker push balrogcpp/clang-linux \
&& docker push balrogcpp/clang-cross \


exit 0
