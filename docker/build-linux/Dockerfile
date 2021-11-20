FROM balrogcpp/deps-linux:latest

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


RUN cmake -P cmake/flat_zip.cmake


RUN mkdir build-linux && cd build-linux \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target build-package \
    && cd .. \
    && rm -rf build-linux
