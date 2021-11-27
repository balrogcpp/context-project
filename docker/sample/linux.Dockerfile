FROM registry.gitlab.com/balrogcpp/context-project/clang-linux

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./sources ./sources
COPY ./deploy ./deploy
COPY ./cmake ./cmake
COPY ./sample ./sample
COPY ./LICENSE .
COPY ./programs ./programs
COPY ./assets ./assets
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./thirdparty/CMakeLists.txt ./thirdparty/CMakeLists.txt

RUN mkdir -p ${CONTEXT_HOME}/thirdparty/external/Release \
    && cd ${CONTEXT_HOME}/thirdparty/external/Release \
    && wget https://github.com/balrogcpp/glue-dep/raw/master/linux-clang-x86_64.tar.xz  -O - | tar -xJ

RUN apt-get update \
    && apt-get install --no-install-recommends -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev && apt-get clean \
    && cmake -P cmake/flat_zip.cmake \
    && mkdir build-linux && cd build-linux \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target build-package \
    && rm -rf ../build-linux
