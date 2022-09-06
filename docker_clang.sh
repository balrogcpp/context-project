#!/bin/bash

docker build -t balrogcpp/cross_clang . -f cross_clang.Dockerfile
#docker push balrogcpp/cross_clang
