FROM registry.gitlab.com/balrogcpp/context-project/clang-apple

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
COPY ./Binaries ./Binaries
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./ThirdParty/CMakeLists.txt ./ThirdParty/CMakeLists.txt

RUN cmake -P CMake/FlatZipAssets.cmake

RUN mkdir -p ${CONTEXT_HOME}/ThirdParty/External \
    && tar Jxfp ${CONTEXT_HOME}/Binaries/Dependencies/Darwin_x86_64_Clang_Release.tar.xz -C ${CONTEXT_HOME}/ThirdParty/External

RUN mkdir build-apple && cd build-apple \
    && eval `x86_64-apple-darwin19-osxcross-conf` \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target BuildPackage \
    && rm -rf ../build-apple
