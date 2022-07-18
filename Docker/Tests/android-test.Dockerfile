# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM registry.gitlab.com/balrogcpp/context-project/clang-android

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Engine/Binaries ./Engine/Binaries

RUN mkdir -p ${CONTEXT_HOME}/Engine/Dependencies/External \
    && tar Jxfp ${CONTEXT_HOME}/Engine/Binaries/Dependencies/Android_aarch64_Clang_Release.tar.xz -C ${CONTEXT_HOME}/Engine/Dependencies/External

COPY ./Engine ./Engine
COPY ./Example ./Example
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN cmake -P CMake/FlatZipAssets.cmake \
    && cd Engine/Android \
    && ./gradlew assembleRelease \
    && cd ../../ \
    && mv Engine/Android/app/build/outputs/apk/release/app-arm64-v8a-release.apk Artifacts/GlueSample_Android_aarch64_$GIT_HASH.apk \
    && rm -rf Android ${ANDROID_HOME} /root/.android
