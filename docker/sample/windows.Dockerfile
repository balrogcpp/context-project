FROM registry.gitlab.com/balrogcpp/context-project/clang-windows

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
COPY ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt

RUN mkdir -p ${CONTEXT_HOME}/dependencies/external/Release \
    && cd ${CONTEXT_HOME}/dependencies/external/Release \
    && wget https://github.com/balrogcpp/glue-dep/raw/master/windows-clang-mingw-x86_64.tar.xz  -O - | tar -xJ

RUN cmake -P cmake/flat_zip.cmake \
    && mkdir build-windows && cd build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target build-package \
    && cd .. \
    && rm -rf build-windows
