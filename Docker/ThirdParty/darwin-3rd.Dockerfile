# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM registry.gitlab.com/balrogcpp/context-project/clang-darwin

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Source ./Source
COPY ./ThirdParty/CMakeLists.txt ./ThirdParty/CMakeLists.txt
COPY ./ThirdParty/Patch ./ThirdParty/Patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN mkdir ${CONTEXT_HOME}/build-apple && cd ${CONTEXT_HOME}/build-apple \
    && eval $X86_64_EVAL \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -G Ninja .. \
    && cmake --build . --target ThirdParty

RUN cd ${CONTEXT_HOME}/ThirdParty/External/Darwin_x86_64_Clang_Release \
    && rm -rf src tmp \
    && cd  ${CONTEXT_HOME}/ThirdParty/External \
    && tar cfJ Darwin_x86_64_Clang_Release.tar.xz Darwin_x86_64_Clang_Release \
    && mv Darwin_x86_64_Clang_Release.tar.xz ${CONTEXT_HOME}/Artifacts
