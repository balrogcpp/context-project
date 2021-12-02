FROM registry.gitlab.com/balrogcpp/context-project/clang-apple

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Thirdparty/CMakeLists.txt ./Thirdparty/CMakeLists.txt
COPY ./Thirdparty/Patch ./Thirdparty/Patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN mkdir ${CONTEXT_HOME}/build-apple && cd ${CONTEXT_HOME}/build-apple \
    && eval `x86_64-apple-darwin19-osxcross-conf` \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -G Ninja .. \
    && cmake --build . --target thirdparty \
    && cd ${CONTEXT_HOME}/Thirdparty/External/Release/apple-darwin-clang-x86_64 \
    && rm -rf src tmp ${CONTEXT_HOME}/build-apple

RUN cd  ${CONTEXT_HOME}/Thirdparty/External/Release \
    && tar cfJ apple-darwin-clang-x86_64.tar.xz apple-darwin-clang-x86_64 \
    && mv apple-darwin-clang-x86_64.tar.xz ${CONTEXT_HOME}/Artifacts
