FROM balrogcpp/deps-linux:latest

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


RUN python3 zip-dependencies.py

RUN mkdir build-linux && cd build-linux \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target package \
    && cd .. \
    && rm -rf build-linux
