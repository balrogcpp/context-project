# This file is part of Glue Engine. Created by Andrey Vasiliev


FROM registry.gitlab.com/balrogcpp/context-project/clang-android


ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}


RUN mkdir -p ${CONTEXT_HOME}/Engine/Dependencies/External \
    && cd ${CONTEXT_HOME}/Engine/Dependencies/External \
    && wget https://github.com/balrogcpp/glue-deps/raw/master/Android_aarch64_Clang_Release.tar.xz -O - | tar -xJ


COPY ./Engine ./Engine
COPY ./Example ./Example
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./FlatZipAssets.cmake ./FlatZipAssets.cmake
COPY ./CMake ./CMake


RUN cmake -P FlatZipAssets.cmake \
    && cd Engine/Android \
    && ./gradlew assembleRelease \
    && cd ../../ \
    && mv Engine/Android/app/build/outputs/apk/release/app-arm64-v8a-release.apk Artifacts/GlueSample_Android_aarch64_$GIT_HASH.apk \
    && rm -rf Android ${ANDROID_HOME} /root/.android
