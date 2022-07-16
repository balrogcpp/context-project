# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM ubuntu:18.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get --no-install-recommends -y install git zip unzip xz-utils wget ca-certificates \
    && apt-get clean

ARG CMAKE_VERSION=3.23.2
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

RUN apt-get update \
    && apt-get install -y wget ca-certificates gnupg2 apt-transport-https \
    && echo 'deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic main' >> /etc/apt/sources.list \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && echo 'deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu bionic main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 60C317803A41BA51845E371A1E9377A2BA9EF27F \
    && apt-get update \
    && apt-get -y install --no-install-recommends llvm clang lld make autoconf file patch \
    && apt-get clean

ARG OSXCROSS_ROOT=/opt/osxcross
ARG MACOS_SDK_VERSION=12.3
ARG MACOS_SDK_CODE=21.4
RUN apt-get update \
    && apt-get install --no-install-recommends -y libxml2 lzma-dev libxml2-dev libssl-dev python \
    && apt-get clean \
    && git clone https://github.com/tpoechtrager/osxcross.git \
    && wget https://github.com/balrogcpp/MacOSXsdk/raw/master/MacOSX${MACOS_SDK_VERSION}.sdk.tar.xz -O ./osxcross/tarballs/MacOSX${MACOS_SDK_VERSION}.sdk.tar.xz \
    && cd osxcross \
    && git checkout 17bb5e2d0a46533c1dd525cf4e9a80d88bd9f00e \
    && UNATTENDED=1 TARGET_DIR=${OSXCROSS_ROOT} ./build.sh \
    && cd .. \
    && rm -rf osxcross \
    && apt-get -y purge lzma-dev libxml2-dev libssl-dev python \
    && apt-get -y autoremove
ENV OSXCROSS_HOST=x86_64-apple-darwin${MACOS_SDK_CODE}
ENV OSXCROSS_TOOLCHAIN_FILE=${OSXCROSS_ROOT}/toolchain.cmake
ENV PATH="${OSXCROSS_ROOT}/bin:${PATH}"
ENV X86_64_EVAL=`x86_64-apple-darwin${MACOS_SDK_CODE}-osxcross-conf`
ENV ARM64_EVAL=`arm64-apple-darwin${MACOS_SDK_CODE}-osxcross-conf`
