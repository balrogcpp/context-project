FROM balrogcpp/clang-android:latest
ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./thirdparty/CMakeLists.txt ./thirdparty/CMakeLists.txt
COPY ./thirdparty/patch ./thirdparty/patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./doc ./doc
COPY ./cmake ./cmake
COPY ./android ./android

RUN cd ./android \
    && ./gradlew assembleRelease \
    && cd ${CONTEXT_HOME}/thirdparty/external/Release/android-clang-aarch64 \
    && rm -rf src tmp ${CONTEXT_HOME}/android ${ANDROID_HOME}/emulator ${ANDROID_HOME}/tools /root/.android /root/.gradle

RUN cd  ${CONTEXT_HOME}/thirdparty/external/Release \
    && tar cfJ android-clang-aarch64.tar.xz android-clang-aarch64 \
    && mv android-clang-aarch64.tar.xz ${CONTEXT_HOME}/artifacts
