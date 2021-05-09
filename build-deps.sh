#!/bin/bash


docker build . -f docker/clang-linux/Dockerfile -t balrogcpp/clang-linux \
&& docker build . -f docker/clang-cross/Dockerfile -t balrogcpp/clang-cross \
&& docker build . -f docker/deps-cross/Dockerfile -t balrogcpp/deps-cross \
&& docker build . -f docker/deps-linux/Dockerfile -t balrogcpp/deps-linux \
&& docker login \
&& docker push balrogcpp/clang-linux \
&& docker push balrogcpp/clang-cross \
&& docker push balrogcpp/deps-linux \
&& docker push balrogcpp/deps-cross \


exit 0
