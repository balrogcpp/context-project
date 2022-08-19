# This file is part of Glue Engine. Created by Andrey Vasiliev


FROM registry.gitlab.com/balrogcpp/context-project/clang-darwin


ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}


RUN mkdir -p ${CONTEXT_HOME}/Engine/Dependencies/External \
    && cd ${CONTEXT_HOME}/Engine/Dependencies/External \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Darwin_x86_64_Clang_Release.tar.xz -O - | tar -xJ


COPY ./Engine ./Engine
COPY ./Example ./Example
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./FlatZipAssets.cmake ./FlatZipAssets.cmake
COPY ./CMake ./CMake


RUN mkdir build-apple && cd build-apple \
    && eval $X86_64_EVAL \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target BuildPackage \
    && rm -rf ../build-apple
