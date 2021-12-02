#!/bin/bash

docker build . -f linux-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-linux \
&& docker build . -f windows-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-windows \
&& docker build . -f apple-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-apple \
&& docker build . -f android-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-android

#docker push registry.gitlab.com/balrogcpp/context-project/clang-linux \
#&& docker push registry.gitlab.com/balrogcpp/context-project/clang-windows \
#&& docker push registry.gitlab.com/balrogcpp/context-project/clang-apple \
#&& docker push registry.gitlab.com/balrogcpp/context-project/clang-android
