#!/bin/bash

docker build -t registry.gitlab.com/balrogcpp/context-project/clang-cross . -f cross-clang.Dockerfile &&\
docker push registry.gitlab.com/balrogcpp/context-project/clang-cross

exit 0
