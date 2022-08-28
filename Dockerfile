# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM registry.gitlab.com/balrogcpp/context-project/clang-cross

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt
ARG DEPS_DIR=${CONTEXT_HOME}/Dependencies
WORKDIR ${CONTEXT_HOME}


COPY ./Source ./Source
COPY ./Dependencies ./Dependencies
COPY ./Example ./Example
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake


# Linux
RUN apt-get update \
    && apt-get install --no-install-recommends -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get clean

RUN cd ${DEPS_DIR} \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Linux_x86_64_Clang_Release.tar.xz -O - | tar -xJ

RUN mkdir build-linux && cd build-linux \
    && cmake -G Ninja .. \
    && ninja package \
    && rm -rf ../Artifacts/_CPack_Packages \
    && rm -rf ../build-linux


# Win32
RUN cd ${DEPS_DIR} \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Windows_x86_64_Clang_Mingw_Release.tar.xz -O - | tar -xJ

RUN mkdir build-windows && cd build-windows \
    && cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-mingw.cmake -G Ninja .. \
    && ninja package \
    && rm -rf ../Artifacts/_CPack_Packages \
    && rm -rf ../build-windows


# Apple
RUN cd ${DEPS_DIR} \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Darwin_x86_64_Clang_Release.tar.xz -O - | tar -xJ

RUN mkdir build-apple && cd build-apple \
    && eval $X86_64_EVAL \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${OSXCROSS_TOOLCHAIN_FILE} -G Ninja .. \
    && ninja package \
    && rm -rf ../Artifacts/_CPack_Packages \
    && rm -rf ../build-apple


# wasm
ARG EMSDK_ROOT=/opt/emsdk
ARG EMSDK_VERSION=3.1.19
RUN apt-get update \
    && apt-get --no-install-recommends -y install python3 \
    && apt-get clean \
    && cd /opt \
    && git clone --recursive -b ${EMSDK_VERSION} --depth 1 https://github.com/emscripten-core/emsdk.git \
    && cd emsdk \
    && rm -rf .git \
    && ./emsdk install latest \
    && ./emsdk activate latest \
    && . ./emsdk_env.sh
ENV EMSDK_EVAL=${EMSDK_ROOT}/emsdk_env.sh

RUN cd ${DEPS_DIR} \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Emscripten_x86_Clang_Release.tar.xz -O - | tar -xJ

RUN mkdir ${CONTEXT_HOME}/build-wasm && cd ${CONTEXT_HOME}/build-wasm \
    && cd ${EMSDK_ROOT} && . ./emsdk_env.sh \
    && cd ${CONTEXT_HOME}/build-wasm \
    && emcmake cmake -DCMAKE_BUILD_TYPE=Release -G Ninja .. \
    && emmake ninja package \
    && rm -rf ../Artifacts/_CPack_Packages \
    && rm -rf ../build-windows


# Android
ARG ANDROID_HOME=/opt/android-sdk
ARG ANDROID_CMD_VERSION=8512546
RUN apt-get update \
    && apt-get -y install --no-install-recommends openjdk-11-jdk \
    && apt-get clean \
    && cd /opt \
    && wget https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMD_VERSION}_latest.zip -O tools.zip \
    && unzip tools.zip \
    && rm tools.zip \
    && cd cmdline-tools/bin \
    && yes | ./sdkmanager  --licenses --sdk_root=${ANDROID_HOME} \
    && ./sdkmanager  --install "cmake;3.18.1" --sdk_root=${ANDROID_HOME} \
    && cd ../../ \
    && rm -rf /root/.android /root/.gradle
ENV PATH="/opt/cmdline-tools/bin:${PATH}"
ENV ANDROID_SDK_ROOT="${ANDROID_HOME}"

RUN cd ${DEPS_DIR} \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Android_aarch64_Clang_Release.tar.xz -O - | tar -xJ

RUN cd ${CONTEXT_HOME}/Source/Engine \
    && ./gradlew assembleRelease \
    && rm -rf ${ANDROID_HOME} /root/.android
