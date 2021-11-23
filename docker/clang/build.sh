#!/bin/bash

docker build . -f linux-clang.Dockerfile -t balrogcpp/clang-linux \
&& docker build . -f windows-clang.Dockerfile -t balrogcpp/clang-windows \
&& docker build . -f apple-clang.Dockerfile -t balrogcpp/clang-apple \
&& docker build . -f android-clang.Dockerfile -t balrogcpp/clang-android
