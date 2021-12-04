FROM registry.gitlab.com/balrogcpp/context-project/clang-android

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Thirdparty/CMakeLists.txt ./Thirdparty/CMakeLists.txt
COPY ./Thirdparty/Patch ./Thirdparty/Patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake
COPY ./Android ./Android

RUN cd ./Android \
    && ./gradlew assembleRelease \
    && rm -rf ${CONTEXT_HOME}/android ${ANDROID_HOME}/emulator ${ANDROID_HOME}/tools /root/.android /root/.gradle

RUN cd ${CONTEXT_HOME}/Thirdparty/External/Android_aarch64_Clang_Release \
    && rm -rf src tmp \
    && cd  ${CONTEXT_HOME}/Thirdparty/External \
    && tar cfJ Android_aarch64_Clang_Release.tar.xz Android_aarch64_Clang_Release \
    && mv Android_aarch64_Clang_Release.tar.xz ${CONTEXT_HOME}/Artifacts
