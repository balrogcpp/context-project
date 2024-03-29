FROM balrogcpp/clang_cross:latest


ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/var/build
ARG DEPS_DIR=${CONTEXT_HOME}/contrib
WORKDIR ${CONTEXT_HOME}


# https://stackoverflow.com/questions/38378914/how-to-fix-git-error-rpc-failed-curl-56-gnutls
RUN apt-get update \
    && apt-get -y install git \
    && apt-get clean \
    && git config --global http.postBuffer 1048576000 \
    && git config --global https.postBuffer 1048576000 \
    && git config --global core.compression -1


# cmake ninja upx
ARG CMAKE_VERSION=3.28.3
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
ARG NINJA_VERSION=1.11.1
ARG UPX_VERSION=4.2.3
RUN cd /tmp \
    && wget -q https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip \
    && unzip -q /tmp/ninja-linux.zip -d /usr/local/bin && rm ninja-linux.zip \
    && wget -q https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && mkdir ${CMAKE_HOME} && sh cmake-${CMAKE_VERSION}-Linux-x86_64.sh --skip-license --prefix=${CMAKE_HOME} && rm cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && wget https://github.com/upx/upx/releases/download/v${UPX_VERSION}/upx-${UPX_VERSION}-amd64_linux.tar.xz -q  -O - | tar -xJ \
    && mv upx-${UPX_VERSION}-amd64_linux/upx /usr/local/bin && rm -rf upx-${UPX_VERSION}-amd64_linux
ENV PATH="${CMAKE_HOME}/bin:${PATH}"


# protobuf
ARG PROTOC_VERSION=25.3
RUN cd /tmp \
    && wget -q https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOC_VERSION}/protoc-${PROTOC_VERSION}-linux-x86_64.zip \
    && unzip -q -p /tmp/protoc-${PROTOC_VERSION}-linux-x86_64.zip bin/protoc > /usr/local/bin/protoc && rm -rf /tmp/protoc-${PROTOC_VERSION}-linux-x86_64.zip \
    && chmod +x /usr/local/bin/protoc


COPY ./source ./source
COPY ./contrib ./contrib
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./cmake ./cmake
COPY ./.git ./.git


# linux x86_64
RUN apt-get update \
    && apt-get -y install --no-install-recommends libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get clean \
    && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux-x86_64.cmake -G Ninja .. \
    && cmake --build . --target Contrib \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux-x86_64.cmake -G Ninja .. \
    && cmake --build . --target package \
    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build \
    && apt-get -y purge libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get -y autoremove


# win32
RUN mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw.cmake -G Ninja .. \
    && cmake --build . --target Contrib \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw.cmake -G Ninja .. \
    && cmake --build . --target package \
    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# apple x86_64
RUN mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple-x86_64.cmake -G Ninja .. \
    && cmake --build . --target Contrib \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple-x86_64.cmake -G Ninja .. \
    && cmake --build . --target package \
    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# apple aarch64
#RUN mkdir build && cd build \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple-aarm64.cmake -G Ninja .. \
#    && cmake --build . --target Contrib \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple-aarm64.cmake -G Ninja .. \
#    && cmake --build . --target package \
#    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# android
ARG ANDROID_HOME=/opt/android-sdk
ARG ANDROID_CMD_VERSION=9477386
ARG ANDROID_JAVA_MAJOR=11
ARG ANDROID_CMAKE_VERSION=3.18.1
RUN apt-get update \
    && apt-get -y install --no-install-recommends openjdk-${ANDROID_JAVA_MAJOR}-jdk \
    && apt-get clean \
    && cd /opt \
    && wget https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMD_VERSION}_latest.zip -q -O tools.zip \
    && unzip -q tools.zip && rm tools.zip \
    && yes | ./cmdline-tools/bin/sdkmanager  --licenses --sdk_root=${ANDROID_HOME} > /dev/null \
    && export PATH="/opt/cmdline-tools/bin:${PATH}" \
    && export ANDROID_SDK_ROOT=${ANDROID_HOME} \
    && sdkmanager  --install "cmake;${ANDROID_CMAKE_VERSION}" --sdk_root=${ANDROID_HOME}  \
    && cd ${CONTEXT_HOME} && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux.cmake -G Ninja .. \
    && cmake --build . --target GradleContrib \
    && cmake --build . --target GradleBuild \
    && rm -rf build ../contrib/build ../contrib/sdk /root/.android /root/.gradle ${ANDROID_HOME} \
    && apt-get -y purge openjdk-${ANDROID_JAVA_MAJOR}-jdk \
    && apt-get -y autoremove


# wasm
#ARG EMSDK_ROOT=/opt/emsdk
#ARG EMSDK_VERSION=3.1.26
#RUN apt-get update \
#    && apt-get -y install --no-install-recommends python \
#    && apt-get clean \
#    && cd /opt \
#    && git clone --recursive -b ${EMSDK_VERSION} --depth 1 https://github.com/emscripten-core/emsdk.git \
#    && cd emsdk \
#    && rm -rf .git \
#    && ./emsdk install latest \
#    && ./emsdk activate latest
#ENV EMSDK_EVAL=${EMSDK_ROOT}/emsdk_env.sh
#RUN cd ${EMSDK_ROOT} && EMSDK_QUIET=1 . ./emsdk_env.sh \
#    && mkdir ${CONTEXT_HOME}/build && cd ${CONTEXT_HOME}/build \
#    && emcmake cmake -DCMAKE_BUILD_TYPE=Release -G Ninja .. \
#    && emmake cmake --build . --target Contrib \
#    && emcmake cmake -DCMAKE_BUILD_TYPE=Release -G Ninja .. \
#    && emmake cmake --build . --target package \
#    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build
