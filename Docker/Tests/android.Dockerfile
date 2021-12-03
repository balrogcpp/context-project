FROM registry.gitlab.com/balrogcpp/context-project/clang-android

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Source ./Source
COPY ./Deploy ./Deploy
COPY ./CMake ./CMake
COPY ./LICENSE .
COPY ./Programs ./Programs
COPY ./Assets ./Assets
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./Thirdparty/CMakeLists.txt ./Thirdparty/CMakeLists.txt
COPY ./Android ./Android

RUN mkdir -p ${CONTEXT_HOME}/Thirdparty/External/Release \
    && cd ${CONTEXT_HOME}/Thirdparty/External/Release \
    && wget https://github.com/balrogcpp/glue-dep/raw/master/Android_aarch64_Clang.tar.xz  -O - | tar -xJ

RUN cmake -P CMake/FlatZipAssets.cmake

RUN cd Android \
    && ./gradlew assembleRelease \
    && cd ../ \
    && mv Android/app/build/outputs/apk/release/app-arm64-v8a-release.apk Artifacts/app-arm64-v8a-$GIT_HASH.apk \
    && rm -rf Android ${ANDROID_HOME} /root/.android /root/.gradle
