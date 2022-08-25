#!/bin/bash

docker build . -f cross-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-cross \
&& docker push registry.gitlab.com/balrogcpp/context-project/clang-cross
