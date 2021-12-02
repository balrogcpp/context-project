FROM registry.gitlab.com/balrogcpp/context-project/clang-apple

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Source ./Source
COPY ./Deploy ./Deploy
COPY ./CMake ./CMake
COPY ./LICENSE .
COPY ./Programs ./Programs
COPY ./Assets ./Assets
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./Thirdparty/CMakeLists.txt ./Thirdparty/CMakeLists.txt
COPY ./Android ./Android

RUN mkdir -p ${CONTEXT_HOME}/Thirdparty/External/Release \
    && cd ${CONTEXT_HOME}/Thirdparty/External/Release \
    && wget https://github.com/balrogcpp/glue-dep/raw/master/apple-darwin-clang-x86_64.tar.xz  -O - | tar -xJ

RUN cmake -P CMake/flat_zip.cmake

RUN mkdir build-apple && cd build-apple \
    && eval `x86_64-apple-darwin19-osxcross-conf` \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target build-package \
    && rm -rf ../build-apple
