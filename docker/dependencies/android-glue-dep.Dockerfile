FROM balrogcpp/clang-android:latest
ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
COPY ./dependencies/patch ./dependencies/patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./doc ./doc
COPY ./cmake ./cmake
COPY ./android ./android

RUN cd ./android \
    && ./gradlew assembleRelease \
    && cd ../ \
    && rm -rf android \
    && cd ${CONTEXT_HOME}/dependencies/external/Release/android-clang-aarch64 \
    && rm -rf src tmp \
    && cd ${ANDROID_HOME} \
    && rm -rf emulator tools \
    && rm -rf /root/.android /root/.gradle

RUN cd  ${CONTEXT_HOME}/dependencies/external/Release \
    && tar cfJ android-clang-aarch64.tar.xz android-clang-aarch64 \
    && mv android-clang-aarch64.tar.xz ${CONTEXT_HOME}/artifacts
