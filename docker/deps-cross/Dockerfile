FROM balrogcpp/clang-cross:latest

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}


ADD ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
ADD ./dependencies/patch ./dependencies/patch
ADD ./CMakeLists.txt ./CMakeLists.txt
ADD ./doc ./doc
ADD ./cmake ./cmake


RUN mkdir ${CONTEXT_HOME}/build-windows && cd ${CONTEXT_HOME}/build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw.cmake -G Ninja .. \
    && cmake --build . --target target-deps \
    && cd ${CONTEXT_HOME}/dependencies/external/Release/windows-clang-mingw-x86_64 \
    && rm -rf src tmp \
    && rm -rf ${CONTEXT_HOME}/build-windows


RUN mkdir ${CONTEXT_HOME}/build-apple && cd ${CONTEXT_HOME}/build-apple \
    && eval `x86_64-apple-darwin19-osxcross-conf` \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple.cmake -G Ninja .. \
    && cmake --build . --target target-deps \
    && cd ${CONTEXT_HOME}/dependencies/external/Release/apple-darwin-clang-x86_64 \
    && rm -rf src tmp ${CONTEXT_HOME}/build-apple \
    && rm -rf ${CONTEXT_HOME}/build-apple


ENV CMAKE_VERSION=3.19.8
ENV CMAKE_HOME=/opt/cmake-3.19.8
ENV PATH="${CMAKE_HOME}/bin:${PATH}"

RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir ${CMAKE_HOME} \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh

ADD ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
ADD ./dependencies/patch ./dependencies/patch
ADD ./CMakeLists.txt ./CMakeLists.txt
ADD ./doc ./doc
ADD ./cmake ./cmake
ADD ./android ./android

RUN cd ./android \
    && ./gradlew assembleRelease \
    && rm -rf app/.cxx \
    && cd ${CONTEXT_HOME}/dependencies/external/Release/android-clang-aarch64 \
    && rm -rf src tmp \
    && cd ${ANDROID_HOME} \
    && rm -rf emulator tools
