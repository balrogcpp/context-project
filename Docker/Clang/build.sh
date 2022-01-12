#!/bin/bash

#docker build . -f ubuntu-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-ubuntu \
#&& docker build . -f windows-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-windows \
#&& docker build . -f apple-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-apple \
#&& docker build . -f android-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-android

docker build . -f unicross-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/unicross-clang

#docker push registry.gitlab.com/balrogcpp/context-project/clang-ubuntu \
#&& docker push registry.gitlab.com/balrogcpp/context-project/clang-windows \
#&& docker push registry.gitlab.com/balrogcpp/context-project/clang-apple \
#&& docker push registry.gitlab.com/balrogcpp/context-project/clang-android
