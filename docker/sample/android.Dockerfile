FROM registry.gitlab.com/balrogcpp/context-project/clang-android

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./sources ./sources
COPY ./deploy ./deploy
COPY ./cmake ./cmake
COPY ./sample ./sample
COPY ./LICENSE .
COPY ./programs ./programs
COPY ./assets ./assets
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
COPY ./android ./android

RUN mkdir -p ${CONTEXT_HOME}/dependencies/external/Release \
    && cd ${CONTEXT_HOME}/dependencies/external/Release \
    && wget https://github.com/balrogcpp/glue-dep/raw/master/android-clang-aarch64.tar.xz  -O - | tar -xJ

RUN cmake -P cmake/flat_zip.cmake \
    && cd ./android \
    && ./gradlew assembleRelease \
    && cd ../ \
    && mv android/app/build/outputs/apk/release/app-arm64-v8a-release.apk artifacts/app-arm64-v8a-$GIT_HASH.apk \
    && rm -rf android ${CONTEXT_HOME}/dependencies/external ${ANDROID_HOME} /root/.android /root/.gradle
