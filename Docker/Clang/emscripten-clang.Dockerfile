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


RUN apt-get update \
    && apt-get -y install --no-install-recommends libgcc-7-dev libstdc++-7-dev \
    && apt-get clean


WORKDIR /opt

ARG EMSDK_ROOT=/opt/emsdk
ARG EMSDK_VERSION=3.1.16
RUN apt-get update \
    && apt-get --no-install-recommends -y install python3 \
    && apt-get clean \
    && git clone --recursive -b ${EMSDK_VERSION} --depth 1 https://github.com/emscripten-core/emsdk.git \
    && cd emsdk \
    && rm -rf .git \
    && ./emsdk install latest \
    && ./emsdk activate latest \
    && . ./emsdk_env.sh
ENV EMSDK_EVAL=`${EMSDK_ROOT}/emsdk_env.sh`
