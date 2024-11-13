FROM balrogcpp/clang_cross:latest


ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/var/build
ARG GIT_SHA
ENV GIT_SHA_SHORT=${GIT_SHA}
WORKDIR ${CONTEXT_HOME}


# cmake ninja upx
ARG CMAKE_VERSION=3.29.6
ARG NINJA_VERSION=1.12.1
RUN apt-get update \
    && apt-get -y install --no-install-recommends git strip-nondeterminism \
    && apt-get clean \
    && git config --global http.postBuffer 1048576000 \
    && git config --global https.postBuffer 1048576000 \
    && git config --global core.compression -1 \
    && cd /tmp \
    && wget -q https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip \
    && unzip -q /tmp/ninja-linux.zip -d /usr/local/bin && rm ninja-linux.zip \
    && wget -q https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && mkdir /usr/local && sh cmake-${CMAKE_VERSION}-Linux-x86_64.sh --skip-license --prefix=${CMAKE_HOME} && rm cmake-${CMAKE_VERSION}-Linux-x86_64.sh


COPY ./Source ./Source
COPY ./External ./External
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake


# linux x86_64
RUN apt-get update \
    && apt-get -y install --no-install-recommends libx11-dev libxrandr-dev libglew-dev \
    && apt-get clean \
    && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux-x64.cmake -G Ninja .. \
    && cmake --build . --config Release --target package \
    && rm -rf ../Artifacts/_CPack_Packages ../External/Build ../External/Sdk ../build \
    && apt-get -y purge libx11-dev libxrandr-dev libglew-dev \
    && apt-get -y autoremove --purge \
    && apt-get clean


# win32
RUN mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-mingw-x64.cmake -G Ninja .. \
    && cmake --build . --config Release --target package \
    && rm -rf ../Artifacts/_CPack_Packages ../External/Build ../External/Sdk ../build


# apple x86_64
#RUN mkdir build && cd build \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple-x64.cmake -G Ninja .. \
#    && cmake --build . --config Release --target package \
#    && rm -rf ../Artifacts/_CPack_Packages ../External/Build ../External/Sdk ../build


# apple aarch64
#RUN mkdir build && cd build \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple-aarm64.cmake -G Ninja .. \
#    && cmake --build . --config Release --target package \
#    && rm -rf ../Artifacts/_CPack_Packages ../External/Build ../External/Sdk ../build


# android
#ARG ANDROID_CMD_VERSION=11076708
#ARG ANDROID_JAVA_MAJOR=17
#ENV ANDROID_HOME=/opt/android-sdk
#RUN apt-get update \
#    && apt-get -y install --no-install-recommends openjdk-${ANDROID_JAVA_MAJOR}-jdk \
#    && mkdir $ANDROID_HOME && cd $ANDROID_HOME \
#    && wget https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMD_VERSION}_latest.zip -q -O tools.zip \
#    && unzip -q tools.zip && rm tools.zip \
#    && yes | ./cmdline-tools/bin/sdkmanager  --licenses --sdk_root=$ANDROID_HOME > /dev/null \
#    && cd ${CONTEXT_HOME} && mkdir build && cd build \
#    && cmake -DCMAKE_BUILD_TYPE=Release -DDISABLE_DEPENDENCIES_BUILD=ON -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux-x64.cmake -G Ninja .. \
#    && cmake --build . --config Release --target Gradle \
#    && cmake --build . --config Release --target GradleClear \
#    && rm -rf build ../External/Build ../External/Sdk /root/.android /root/.gradle $ANDROID_HOME \
#    && apt-get -y purge openjdk-${ANDROID_JAVA_MAJOR}-jdk \
#    && apt-get -y autoremove --purge \
#    && apt-get clean
