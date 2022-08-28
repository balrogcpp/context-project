#!/bin/bash

docker build -t registry.gitlab.com/balrogcpp/context-project/clang-cross . -f cross_clang.Dockerfile &&\
docker push registry.gitlab.com/balrogcpp/context-project/clang-cross

exit 0
