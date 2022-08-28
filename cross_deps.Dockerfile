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
    && apt-get install --no-install-recommends -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev \
    && apt-get clean \
    && mkdir ${CONTEXT_HOME}/build-linux && cd ${CONTEXT_HOME}/build-linux \
    && cmake -G Ninja .. \
    && ninja Dependencies \
    && cd ${DEPS_DIR}/Linux_x86_64_Clang_Release \
    && rm -rf src tmp \
    && cd ${DEPS_DIR} \
    && tar cfJ Linux_x86_64_Clang_Release.tar.xz Linux_x86_64_Clang_Release \
    && rm -rf Linux_x86_64_Clang_Release


# Win32
RUN mkdir ${CONTEXT_HOME}/build-windows && cd ${CONTEXT_HOME}/build-windows \
    && cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-mingw.cmake -G Ninja .. \
    && ninja Dependencies \
    && cd ${DEPS_DIR}/Windows_x86_64_Clang_Mingw_Release \
    && rm -rf src tmp \
    && cd ${DEPS_DIR} \
    && tar cfJ Windows_x86_64_Clang_Mingw_Release.tar.xz Windows_x86_64_Clang_Mingw_Release \
    && rm -rf Windows_x86_64_Clang_Mingw_Release


# Apple
RUN mkdir ${CONTEXT_HOME}/build-apple && cd ${CONTEXT_HOME}/build-apple \
    && eval $X86_64_EVAL \
    && cmake -DCMAKE_TOOLCHAIN_FILE=${OSXCROSS_TOOLCHAIN_FILE} -G Ninja .. \
    && cmake --build . --target Dependencies \
    && cd ${DEPS_DIR}/Darwin_x86_64_Clang_Release \
    && rm -rf src tmp \
    && cd ${DEPS_DIR} \
    && tar cfJ Darwin_x86_64_Clang_Release.tar.xz Darwin_x86_64_Clang_Release \
    && rm -rf Darwin_x86_64_Clang_Release


# wasm
ARG EMSDK_ROOT=/opt/emsdk
ARG EMSDK_VERSION=3.1.19
ENV EMSDK_EVAL=${EMSDK_ROOT}/emsdk_env.sh
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

RUN mkdir build-wasm && cd build-wasm \
    && cd ${EMSDK_ROOT} \
    && . ./emsdk_env.sh \
    && cd ${CONTEXT_HOME}/build-wasm \
    && emcmake cmake -G Ninja .. \
    && emmake ninja Dependencies \
    && cd ${DEPS_DIR}/Emscripten_x86_Clang_Release \
    && rm -rf src tmp \
    && cd ${DEPS_DIR} \
    && tar cfJ Emscripten_x86_Clang_Release.tar.xz Emscripten_x86_Clang_Release \
    && rm -rf Emscripten_x86_Clang_Release


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

RUN cd ./Source/Engine \
    && ./gradlew assembleRelease \
    && rm -rf ${CONTEXT_HOME}/android ${ANDROID_HOME}/emulator ${ANDROID_HOME}/tools /root/.android /root/.gradle \
    && cd ${DEPS_DIR}/Android_aarch64_Clang_Release \
    && rm -rf src tmp \
    && cd ${DEPS_DIR} \
    && tar cfJ Android_aarch64_Clang_Release.tar.xz Android_aarch64_Clang_Release \
    && rm -rf Android_aarch64_Clang_Release
