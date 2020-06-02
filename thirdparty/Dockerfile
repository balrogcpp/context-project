FROM balrogcpp/mingw-gcc10-cmake:latest

ARG CONTEXT_HOME=/mnt/context-demo
WORKDIR ${CONTEXT_HOME}

ADD ./thirdparty/CMakeLists.txt ./thirdparty/CMakeLists.txt
ADD CMakeLists.txt .
ADD ./CMake ./CMake

RUN mkdir -p ./build-windows && mkdir -p ./build-linux && mkdir -p ./build-android

WORKDIR ${CONTEXT_HOME}/build-windows
RUN cmake -DCONTEXT_ONLY_DEPS=true -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-mingw.cmake -G Ninja ..
RUN cmake --build . --target context-deps

WORKDIR ${CONTEXT_HOME}/build-linux
RUN cmake -DCONTEXT_ONLY_DEPS=true -G Ninja ..
RUN cmake --build . --target context-deps