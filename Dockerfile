FROM balrogcpp/clang_cross:latest


ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/var/build
ARG DEPS_DIR=${CONTEXT_HOME}/contrib
ARG GIT_SHA
ENV GIT_SHA_SHORT=${GIT_SHA}
WORKDIR ${CONTEXT_HOME}


# cmake ninja upx
ARG CMAKE_VERSION=3.29.6
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
ARG NINJA_VERSION=1.12.1
ARG PROTOC_VERSION=27.3
ARG UPX_VERSION=4.2.4
RUN apt-get update \
    && apt-get -y install --no-install-recommends git \
    && apt-get clean \
    && git config --global http.postBuffer 1048576000 \
    && git config --global https.postBuffer 1048576000 \
    && git config --global core.compression -1 \
    && cd /tmp \
    && wget -q https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip \
    && unzip -q /tmp/ninja-linux.zip -d /usr/local/bin && rm ninja-linux.zip \
    && wget -q https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && mkdir ${CMAKE_HOME} && sh cmake-${CMAKE_VERSION}-Linux-x86_64.sh --skip-license --prefix=${CMAKE_HOME} && rm cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && wget -q https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOC_VERSION}/protoc-${PROTOC_VERSION}-linux-x86_64.zip \
    && unzip -q -p /tmp/protoc-${PROTOC_VERSION}-linux-x86_64.zip bin/protoc > /usr/local/bin/protoc && rm -rf /tmp/protoc-${PROTOC_VERSION}-linux-x86_64.zip \
    && chmod +x /usr/local/bin/protoc \
    && wget https://github.com/upx/upx/releases/download/v${UPX_VERSION}/upx-${UPX_VERSION}-amd64_linux.tar.xz -q  -O - | tar -xJ \
    && mv upx-${UPX_VERSION}-amd64_linux/upx /usr/local/bin && rm -rf upx-${UPX_VERSION}-amd64_linux
ENV PATH="${CMAKE_HOME}/bin:${PATH}"


COPY ./source ./source
COPY ./contrib ./contrib
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./cmake ./cmake


# linux x86_64
RUN apt-get update \
    && apt-get -y install --no-install-recommends libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get clean \
    && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux-x64.cmake -G Ninja .. \
    && cmake --build . --config Release --target Contrib \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux-x64.cmake -G Ninja .. \
    && cmake --build . --config Release --target package \
    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build \
    && apt-get -y purge libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get -y autoremove --purge \
    && apt-get clean


# win32
RUN mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw-x64.cmake -G Ninja .. \
    && cmake --build . --config Release --target Contrib \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw-x64.cmake -G Ninja .. \
    && cmake --build . --config Release --target package \
    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# apple x86_64
#RUN mkdir build && cd build \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple-x64.cmake -G Ninja .. \
#    && cmake --build . --config Release --target Contrib \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple-x64.cmake -G Ninja .. \
#    && cmake --build . --config Release --target package \
#    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# apple aarch64
#RUN mkdir build && cd build \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple-aarm64.cmake -G Ninja .. \
#    && cmake --build . --config Release --target Contrib \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple-aarm64.cmake -G Ninja .. \
#    && cmake --build . --config Release --target package \
#    && rm -rf ../artifacts/_CPack_Packages ../contrib/build ../contrib/sdk ../build


# android
#ARG ANDROID_CMD_VERSION=11076708
#ARG ANDROID_JAVA_MAJOR=17
#ENV ANDROID_HOME=/opt/android-sdk
#RUN apt-get update \
#    && apt-get -y install --no-install-recommends openjdk-${ANDROID_JAVA_MAJOR}-jdk \
#    && apt-get clean \
#    && mkdir $ANDROID_HOME && cd $ANDROID_HOME \
#    && wget https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMD_VERSION}_latest.zip -q -O tools.zip \
#    && unzip -q tools.zip && rm tools.zip \
#    && yes | ./cmdline-tools/bin/sdkmanager  --licenses --sdk_root=$ANDROID_HOME > /dev/null \
#    && cd ${CONTEXT_HOME} && mkdir build && cd build \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-linux-x64.cmake -G Ninja .. \
#    && cmake --build . --config Release --target Gradle \
#    && cmake --build . --config Release --target GradleClear \
#    && rm -rf build ../contrib/build ../contrib/sdk /root/.android /root/.gradle $ANDROID_HOME \
#    && apt-get -y purge openjdk-${ANDROID_JAVA_MAJOR}-jdk \
#    && apt-get -y autoremove --purge \
#    && apt-get clean
