# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM registry.gitlab.com/balrogcpp/context-project/clang-linux

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

RUN apt-get update \
    && apt-get install --no-install-recommends -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get clean

COPY ./Engine/Binaries ./Engine/Binaries

RUN mkdir -p ${CONTEXT_HOME}/Engine/ThirdParty/External \
    && tar Jxfp ${CONTEXT_HOME}/Engine/Binaries/Dependencies/Linux_x86_64_Clang_Release.tar.xz -C ${CONTEXT_HOME}/Engine/ThirdParty/External

COPY ./Engine ./Engine
COPY ./Example ./Example
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN mkdir build-linux && cd build-linux \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target BuildPackage \
    && rm -rf ../build-linux
