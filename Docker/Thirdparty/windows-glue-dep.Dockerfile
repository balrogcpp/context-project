FROM registry.gitlab.com/balrogcpp/context-project/clang-windows

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Thirdparty/CMakeLists.txt ./Thirdparty/CMakeLists.txt
COPY ./Thirdparty/Patch ./Thirdparty/Patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN mkdir ${CONTEXT_HOME}/build-windows && cd ${CONTEXT_HOME}/build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-mingw.cmake -G Ninja .. \
    && cmake --build . --target Thirdparty \
    && cd ${CONTEXT_HOME}/Thirdparty/External/Release/windows-clang-mingw-x86_64 \
    && rm -rf src tmp ${CONTEXT_HOME}/build-windows

RUN cd  ${CONTEXT_HOME}/Thirdparty/External/Release \
    && tar cfJ windows-clang-mingw-x86_64.tar.xz windows-clang-mingw-x86_64 \
    && mv windows-clang-mingw-x86_64.tar.xz ${CONTEXT_HOME}/Artifacts
