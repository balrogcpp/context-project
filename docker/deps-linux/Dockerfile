FROM balrogcpp/clang-linux:latest

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}


RUN apt-get update \
    && apt-get install -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev \
    && apt-get clean


COPY ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
COPY ./dependencies/patch ./dependencies/patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./cmake ./cmake


ENV CC=clang
ENV CXX=clang++

RUN mkdir ${CONTEXT_HOME}/build-linux && cd ${CONTEXT_HOME}/build-linux \
     && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux.cmake -G Ninja .. \
     && cmake --build . --target dependencies \
     && cd ${CONTEXT_HOME}/dependencies/external/Release/linux-clang-x86_64 \
     && rm -rf src tmp \
     && rm -rf ${CONTEXT_HOME}/build-linux \
     && cd ${CONTEXT_HOME} \
     && rm -rf CMake CMakeLists.txt dependencies/CMakeLists.txt dependencies/patch
