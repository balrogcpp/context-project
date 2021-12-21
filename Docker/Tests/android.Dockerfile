FROM registry.gitlab.com/balrogcpp/context-project/clang-android

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Source ./Source
COPY ./Deploy ./Deploy
COPY ./Doc ./Doc
COPY ./CMake ./CMake
COPY ./LICENSE .
COPY ./Programs ./Programs
COPY ./Assets ./Assets
COPY ./Binaries ./Binaries
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./ThirdParty/CMakeLists.txt ./ThirdParty/CMakeLists.txt
COPY ./Android ./Android

RUN cmake -P CMake/FlatZipAssets.cmake

RUN mkdir -p ${CONTEXT_HOME}/ThirdParty/External \
    && tar Jxfp ${CONTEXT_HOME}/Binaries/Dependencies/Android_aarch64_Clang_Release.tar.xz -C ${CONTEXT_HOME}/ThirdParty/External

RUN cd Android \
    && ./gradlew assembleRelease \
    && cd ../ \
    && mv Android/app/build/outputs/apk/release/app-arm64-v8a-release.apk Artifacts/app-arm64-v8a-$GIT_HASH.apk \
    && rm -rf Android ${ANDROID_HOME} /root/.android /root/.gradle
