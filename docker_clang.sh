#!/bin/bash

docker build -t cross_clang . -f cross_clang.Dockerfile &&\
docker push cross_clang

exit 0
