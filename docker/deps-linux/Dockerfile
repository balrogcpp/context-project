FROM balrogcpp/clang-linux:latest

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

ADD ./sources ./sources
ADD ./deploy ./deploy
ADD ./cmake ./cmake
ADD ./demo ./demo
ADD ./doc ./doc
ADD ./tests ./tests
ADD ./LICENSE .
ADD ./programs ./programs
ADD ./assets ./assets
ADD ./CMakeLists.txt .
ADD ./zip-dependencies.py .
ADD ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt


RUN apt-get update \
    && apt-get install -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev \
    && apt-get clean


ADD ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
ADD ./dependencies/patch ./dependencies/patch
ADD ./CMakeLists.txt ./CMakeLists.txt
ADD ./doc ./doc
ADD ./cmake ./cmake


ENV CC=clang
ENV CXX=clang++

RUN mkdir ${CONTEXT_HOME}/build-linux && cd ${CONTEXT_HOME}/build-linux \
     && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux.cmake -G Ninja .. \
     && cmake --build . --target target-deps \
     && cd ${CONTEXT_HOME}/dependencies/external/Release/linux-clang-x86_64 \
     && rm -rf src tmp \
     && rm -rf ${CONTEXT_HOME}/build-linux


RUN cd ${CONTEXT_HOME} \
    && rm -rf doc CMake CMakeLists.txt dependencies/CMakeLists.txt dependencies/patch
