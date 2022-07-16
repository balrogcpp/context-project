# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM registry.gitlab.com/balrogcpp/context-project/clang-android

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Engine ./Engine
COPY ./Example ./Example
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN cd ./Engine/Android \
    && ./gradlew assembleRelease \
    && rm -rf ${CONTEXT_HOME}/android ${ANDROID_HOME}/emulator ${ANDROID_HOME}/tools /root/.android /root/.gradle

RUN cd ${CONTEXT_HOME}/Engine/ThirdParty/External/Android_aarch64_Clang_Release \
    && rm -rf src tmp \
    && cd  ${CONTEXT_HOME}/Engine/ThirdParty/External \
    && tar cfJ Android_aarch64_Clang_Release.tar.xz Android_aarch64_Clang_Release \
    && mv Android_aarch64_Clang_Release.tar.xz ${CONTEXT_HOME}/Artifacts
