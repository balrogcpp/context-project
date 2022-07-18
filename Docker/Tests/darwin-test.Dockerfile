# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM registry.gitlab.com/balrogcpp/context-project/clang-darwin

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Engine/Binaries ./Engine/Binaries

RUN mkdir -p ${CONTEXT_HOME}/Engine/Dependencies/External \
    && tar Jxfp ${CONTEXT_HOME}/Engine/Binaries/Dependencies/Darwin_x86_64_Clang_Release.tar.xz -C ${CONTEXT_HOME}/Engine/Dependencies/External

COPY ./Engine ./Engine
COPY ./Example ./Example
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN mkdir build-apple && cd build-apple \
    && eval $X86_64_EVAL \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target BuildPackage \
    && rm -rf ../build-apple
