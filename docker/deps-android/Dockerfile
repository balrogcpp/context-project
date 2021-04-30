FROM balrogcpp/clang-android:latest

ARG DEBIAN_FRONTEND=noninteractive

########################################################################################################################
ENV CMAKE_VERSION=3.19.8
ENV CMAKE_HOME=/opt/cmake
ENV NINJA_VERSION=1.10.2
ENV PATH="${CMAKE_HOME}/bin:${PATH}"

RUN wget https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip -P /tmp && unzip /tmp/ninja-linux.zip -d /usr/local/bin && rm /tmp/ninja-linux.zip \
    && wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir /opt/cmake \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh


########################################################################################################################
ARG CONTEXT_HOME=/mnt/build
WORKDIR ${CONTEXT_HOME}


ADD ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
ADD ./dependencies/patch ./dependencies/patch
ADD ./CMakeLists.txt ./CMakeLists.txt
ADD ./doc ./doc
ADD ./cmake ./cmake
ADD ./android ./android


RUN apt-get update \
    && apt-get install -y gcc g++ \
    && cd ./android \
    && ./gradlew assembleRelease \
    && cd ../ \
    && rm -rf ./android \
    && cd ${CONTEXT_HOME}/dependencies/external/Release/android-clang-aarch64 \
    && rm -rf src tmp \
    && cd ${CONTEXT_HOME}/dependencies/external/Release/android-clang-x86_64 \
    && rm -rf src tmp \
    && rm -rf ${ANDROID_HOME} \
    && apt-get purge -y gcc g++ \
    && apt-get autoremove -y \
    && apt-get clean \
    && yes | sdkmanager  --licenses --sdk_root=${ANDROID_HOME} \
    && cd ../
