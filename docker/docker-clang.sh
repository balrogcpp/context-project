#!/bin/bash

docker build . -f clang-linux/Dockerfile -t balrogcpp/clang-linux \
&& docker build . -f clang-cross/Dockerfile -t balrogcpp/clang-cross

exit 0
