FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /mnt

RUN apt-get update \
    && apt-get install -y wget ca-certificates gnupg2 apt-transport-https \
    && echo 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal main' >> /etc/apt/sources.list \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && apt-get update \
    && apt-get -y install --no-install-recommends llvm clang lld git zip unzip xz-utils make autoconf file patch && apt-get clean

ARG CMAKE_VERSION=3.19.8
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
ARG NINJA_VERSION=1.10.2

RUN wget https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip -P /tmp && unzip /tmp/ninja-linux.zip -d /usr/local/bin && rm /tmp/ninja-linux.zip \
    && wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir ${CMAKE_HOME} \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh

ENV PATH="${CMAKE_HOME}/bin:${PATH}"

ARG OSXCROSS_ROOT=/opt/osxcross
ARG OSXCROSS_HOME=/opt
ARG MACOSX_VERSION=10.15

RUN apt-get install --no-install-recommends -y libxml2 lzma-dev libxml2-dev libssl-dev python && apt-get clean \
    && cd ${OSXCROSS_HOME} \
    && git clone --depth 1 https://github.com/tpoechtrager/osxcross.git \
    && wget https://github.com/balrogcpp/MacOSXsdk/raw/master/MacOSX${MACOSX_VERSION}.sdk.tar.xz -O ${OSXCROSS_ROOT}/tarballs/MacOSX${MACOSX_VERSION}.sdk.tar.xz \
    && cd ${OSXCROSS_ROOT} \
    && UNATTENDED=1 ./build.sh \
    && rm ${OSXCROSS_ROOT}/tarballs/MacOSX${MACOSX_VERSION}.sdk.tar.xz \
    && rm -rf ${OSXCROSS_ROOT}/build \
    && rm -rf `find  . -type d -name .git` \
    && apt-get -y purge lzma-dev libxml2-dev libssl-dev python \
    && apt-get -y autoremove

ENV OSXCROSS_HOST=x86_64-apple-darwin19
ENV OSXCROSS_TOOLCHAIN_FILE=${OSXCROSS_ROOT}/target/toolchain.cmake
ENV PATH="${OSXCROSS_ROOT}/target/bin:${PATH}"
ENV eval `${OSXCROSS_ROOT}/target/bin/x86_64-apple-darwin19-osxcross-conf`
