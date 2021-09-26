FROM balrogcpp/clang-cross:latest

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}


COPY ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
COPY ./dependencies/patch ./dependencies/patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./doc ./doc
COPY ./cmake ./cmake


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


COPY ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
COPY ./dependencies/patch ./dependencies/patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./doc ./doc
COPY ./cmake ./cmake
COPY ./android ./android

RUN cd ./android \
    && ./gradlew assembleRelease \
    && cd ../ \
    && rm -rf android \
    && cd ${CONTEXT_HOME}/dependencies/external/Release/android-clang-aarch64 \
    && rm -rf src tmp \
    && cd ${ANDROID_HOME} \
    && rm -rf emulator tools
