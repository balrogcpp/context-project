# This file is part of Glue Engine. Created by Andrey Vasiliev


FROM registry.gitlab.com/balrogcpp/context-project/clang-cross


ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}


RUN apt-get update \
    && apt-get install --no-install-recommends -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get clean

COPY ./Engine ./Engine
COPY ./Example ./Example
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake


# Linux
RUN mkdir -p ${CONTEXT_HOME}/Engine/Dependencies/External \
    && cd ${CONTEXT_HOME}/Engine/Dependencies/External \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Linux_x86_64_Clang_Release.tar.xz -O - | tar -xJ

RUN mkdir build-linux && cd build-linux \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && ninja package \
    && rm -rf ../Artifacts/_CPack_Packages \
    && rm -rf ../build-linux


# Win32
RUN mkdir -p ${CONTEXT_HOME}/Engine/Dependencies/External \
    && cd ${CONTEXT_HOME}/Engine/Dependencies/External \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Windows_x86_64_Clang_Mingw_Release.tar.xz -O - | tar -xJ

RUN mkdir build-windows && cd build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-mingw.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && ninja package \
    && rm -rf ../Artifacts/_CPack_Packages \
    && rm -rf ../build-windows


# Apple
RUN mkdir -p ${CONTEXT_HOME}/Engine/Dependencies/External \
    && cd ${CONTEXT_HOME}/Engine/Dependencies/External \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Darwin_x86_64_Clang_Release.tar.xz -O - | tar -xJ

RUN mkdir build-apple && cd build-apple \
    && eval $X86_64_EVAL \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && ninja package \
    && rm -rf ../Artifacts/_CPack_Packages \
    && rm -rf ../build-apple


# Android
RUN apt-get update \
    && apt-get -y install --no-install-recommends openjdk-11-jdk \
    && apt-get clean

WORKDIR /opt
ARG ANDROID_HOME=/opt/android-sdk
ARG ANDROID_CMD_VERSION=8512546
RUN wget https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMD_VERSION}_latest.zip -O tools.zip \
    && unzip tools.zip \
    && rm tools.zip \
    && cd cmdline-tools/bin \
    && yes | ./sdkmanager  --licenses --sdk_root=${ANDROID_HOME} \
    && ./sdkmanager  --install "cmake;3.18.1" --sdk_root=${ANDROID_HOME} \
    && cd ../../ \
    && rm -rf /root/.android /root/.gradle
ENV PATH="/opt/cmdline-tools/bin:${PATH}"
ENV ANDROID_SDK_ROOT="${ANDROID_HOME}"

RUN mkdir -p ${CONTEXT_HOME}/Engine/Dependencies/External \
    && cd ${CONTEXT_HOME}/Engine/Dependencies/External \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Android_aarch64_Clang_Release.tar.xz -O - | tar -xJ

RUN cd ${CONTEXT_HOME}/Engine/Source/Android \
    && ./gradlew assembleRelease \
    && cd ../../ \
    && mv ${CONTEXT_HOME}/Engine/Source/Android/app/build/outputs/apk/release/app-arm64-v8a-release.apk ${CONTEXT_HOME}/Artifacts/GlueSample_Android_aarch64_$GIT_HASH.apk \
    && rm -rf Android ${ANDROID_HOME} /root/.android

# wasm
WORKDIR /opt
ARG EMSDK_ROOT=/opt/emsdk
ARG EMSDK_VERSION=3.1.19
RUN apt-get update \
    && apt-get --no-install-recommends -y install python3 \
    && apt-get clean \
    && git clone --recursive -b ${EMSDK_VERSION} --depth 1 https://github.com/emscripten-core/emsdk.git \
    && cd emsdk \
    && rm -rf .git \
    && ./emsdk install latest \
    && ./emsdk activate latest \
    && . ./emsdk_env.sh
ENV EMSDK_EVAL=${EMSDK_ROOT}/emsdk_env.sh

RUN mkdir -p ${CONTEXT_HOME}/Engine/Dependencies/External \
    && cd ${CONTEXT_HOME}/Engine/Dependencies/External \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Emscripten_x86_Clang_Release.tar.xz -O - | tar -xJ

RUN  mkdir ${CONTEXT_HOME}/build-wasm && cd ${CONTEXT_HOME}/build-wasm \
    && cd ${EMSDK_ROOT} && . ./emsdk_env.sh \
    && cd ${CONTEXT_HOME}/build-wasm \
    && emcmake cmake -DCMAKE_BUILD_TYPE=Release -G Ninja .. \
    && emmake ninja package \
    && rm -rf ../Artifacts/_CPack_Packages \
    && rm -rf ../build-windows
