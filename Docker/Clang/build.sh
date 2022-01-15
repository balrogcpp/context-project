#!/bin/bash


docker build . -f android-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-android \
&& docker build . -f unicross-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-unicross

#docker push android-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-android \
#&& docker push registry.gitlab.com/balrogcpp/context-project/clang-unicross
