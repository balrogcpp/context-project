#!/bin/bash

docker build . -f linux-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-linux \
&& docker build . -f windows-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-windows \
&& docker build . -f darwin-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-darwin \
&& docker build . -f emscripten-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-emscripten \
&& docker build . -f android-clang.Dockerfile -t registry.gitlab.com/balrogcpp/context-project/clang-android \
&& docker push registry.gitlab.com/balrogcpp/context-project/clang-linux \
&& docker push registry.gitlab.com/balrogcpp/context-project/clang-windows \
&& docker push registry.gitlab.com/balrogcpp/context-project/clang-darwin \
&& docker push registry.gitlab.com/balrogcpp/context-project/clang-emscripten \
&& docker push registry.gitlab.com/balrogcpp/context-project/clang-android
