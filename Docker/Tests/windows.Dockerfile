FROM registry.gitlab.com/balrogcpp/context-project/clang-windows

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Source ./Source
COPY ./Deploy ./Deploy
COPY ./Doc ./Doc
COPY ./CMake ./CMake
COPY ./LICENSE .
COPY ./Programs ./Programs
COPY ./Assets ./Assets
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./ThirdParty/CMakeLists.txt ./ThirdParty/CMakeLists.txt

RUN mkdir -p ${CONTEXT_HOME}/ThirdParty/External \
    && cd ${CONTEXT_HOME}/ThirdParty/External \
    && wget https://github.com/balrogcpp/glue-dep/raw/master/Windows_x86_64_Clang_Mingw_Release.tar.xz  -O - | tar -xJ

RUN cmake -P CMake/FlatZipAssets.cmake

RUN mkdir build-windows && cd build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-mingw.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target BuildPackage \
    && rm -rf ../build-windows