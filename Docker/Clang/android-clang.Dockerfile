# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM ubuntu:18.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get --no-install-recommends -y install git zip unzip xz-utils wget ca-certificates \
    && apt-get clean

RUN apt-get update \
    && apt-get -y install --no-install-recommends openjdk-11-jdk \
    && apt-get clean

WORKDIR /opt
ARG ANDROID_HOME=/opt/android-sdk
ARG ANDROID_CMD_VERSION=8512546
RUN wget https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMD_VERSION}_latest.zip -O tools.zip \
    && unzip tools.zip \
    && rm tools.zip \
    && cd cmdline-tools/bin \
    && yes | ./sdkmanager  --licenses --sdk_root=${ANDROID_HOME} \
    && cd ../../ \
    && rm -rf /root/.android /root/.gradle
ENV PATH="/opt/tools/bin:${PATH}"
ENV ANDROID_SDK_ROOT="${ANDROID_HOME}"

ARG CMAKE_VERSION=3.18.1
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
ARG NINJA_VERSION=1.11.0
ARG UPX_VERSION=3.96
RUN wget https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip -P /tmp \
    && unzip /tmp/ninja-linux.zip -d /usr/local/bin \
    && rm /tmp/ninja-linux.zip \
    && wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir ${CMAKE_HOME} \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh \
    && wget https://github.com/upx/upx/releases/download/v${UPX_VERSION}/upx-${UPX_VERSION}-amd64_linux.tar.xz  -O - | tar -xJ \
    && cd upx-${UPX_VERSION}-amd64_linux \
    && cp upx /usr/local/bin \
    && cd .. \
    && rm -rf upx-${UPX_VERSION}-amd64_linux
ENV PATH="${CMAKE_HOME}/bin:${PATH}"
