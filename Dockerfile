FROM balrogcpp/cross_clang

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/var/build
ARG DEPS_DIR=${CONTEXT_HOME}/contrib
WORKDIR ${CONTEXT_HOME}


COPY ./source ./source
COPY ./contrib ./contrib
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./cmake ./cmake


# https://stackoverflow.com/questions/38378914/how-to-fix-git-error-rpc-failed-curl-56-gnutls
RUN git config --global http.postBuffer 1048576000 \
    && git config --global https.postBuffer 1048576000 \
    && git config --global core.compression -1


# linux x86_64
RUN apt-get update \
    && apt-get -y install --no-install-recommends libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get clean \
    && mkdir build && cd build \
    && cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux.cmake -G Ninja .. \
    && ninja contrib \
    && cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux.cmake -G Ninja .. \
    && ninja package \
    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build \
    && apt-get -y purge libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get -y autoremove


# win32
RUN mkdir build && cd build \
    && cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw.cmake -G Ninja .. \
    && ninja contrib \
    && cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw.cmake -G Ninja .. \
    && ninja package \
    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# apple x86_64
RUN mkdir build && cd build \
    && eval $X86_64_EVAL \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${OSXCROSS_TOOLCHAIN_FILE} -G Ninja .. \
    && ninja contrib \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${OSXCROSS_TOOLCHAIN_FILE} -G Ninja .. \
    && ninja package \
    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# apple aarch64
#RUN mkdir build && cd build \
#    && export OSXCROSS_HOST=$OSXCROSS_HOST_ARM64 \
#    && eval $ARM64_EVAL \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${OSXCROSS_TOOLCHAIN_FILE} -G Ninja .. \
#    && ninja contrib \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${OSXCROSS_TOOLCHAIN_FILE} -G Ninja .. \
#    && ninja package \
#    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# wasm
#ARG EMSDK_ROOT=/opt/emsdk
#ARG EMSDK_VERSION=3.1.19
#RUN apt-get update \
#    && apt-get -y install --no-install-recommends python3 \
#    && apt-get clean \
#    && cd /opt \
#    && git clone --recursive -b ${EMSDK_VERSION} --depth 1 https://github.com/emscripten-core/emsdk.git \
#    && cd emsdk \
#    && rm -rf .git \
#    && ./emsdk install latest \
#    && ./emsdk activate latest \
#    && . ./emsdk_env.sh
#ENV EMSDK_EVAL=${EMSDK_ROOT}/emsdk_env.sh
#
#RUN mkdir ${CONTEXT_HOME}/build && cd ${CONTEXT_HOME}/build \
#    && cd ${EMSDK_ROOT} && . ./emsdk_env.sh \
#    && cd ${CONTEXT_HOME}/build-wasm \
#    && emcmake cmake -DCMAKE_BUILD_TYPE=Release -G Ninja .. \
#    && emmake ninja contrib \
#    && emcmake cmake -DCMAKE_BUILD_TYPE=Release -G Ninja .. \
#    && emmake ninja package \
#    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# android
#ARG ANDROID_HOME=/opt/android-sdk
#ARG ANDROID_CMD_VERSION=8512546
#RUN apt-get update \
#    && apt-get -y install --no-install-recommends openjdk-11-jdk \
#    && apt-get clean \
#    && cd /opt \
#    && wget https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMD_VERSION}_latest.zip -O tools.zip \
#    && unzip tools.zip \
#    && rm tools.zip \
#    && cd cmdline-tools/bin \
#    && yes | ./sdkmanager  --licenses --sdk_root=${ANDROID_HOME} \
#    && ./sdkmanager  --install "build-tools;30.0.3" --sdk_root=${ANDROID_HOME} \
#    && ./sdkmanager  --install "cmake;3.18.1" --sdk_root=${ANDROID_HOME} \
#    && ./sdkmanager  --install "ndk;25.1.8937393" --sdk_root=${ANDROID_HOME} \
#    && cd ../../ \
#    && rm -rf /root/.android /root/.gradle
#ENV PATH="/opt/cmdline-tools/bin:${PATH}"
#ENV ANDROID_SDK_ROOT="${ANDROID_HOME}"
#
#RUN cd ${CONTEXT_HOME}/source/Engine \
#    && sh gradlew assembleRelease \
#    && rm -rf android/.cxx \
#    && sh gradlew assembleRelease \
#    && cp android/build/outputs/apk/release/android-arm64-v8a-release.apk ${CONTEXT_HOME}/artifacts\
#    && rm -rf ${ANDROID_HOME} /root/.android
