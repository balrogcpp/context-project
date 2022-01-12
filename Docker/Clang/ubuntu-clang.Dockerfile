FROM ubuntu:16.04

ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /mnt

RUN apt-get update \
    && apt-get install -y wget ca-certificates gnupg2 apt-transport-https \
    && echo 'deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main' >> /etc/apt/sources.list \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && echo 'deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu xenial main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 60C317803A41BA51845E371A1E9377A2BA9EF27F \
    && apt-get update \
    && apt-get -y install --no-install-recommends llvm clang lld libc++-13-dev libc++abi-13-dev libomp-13-dev git zip unzip xz-utils make autoconf file patch upx \
    && apt-get clean

ARG CMAKE_VERSION=3.19.8
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
ARG NINJA_VERSION=1.10.2

RUN wget https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip -P /tmp \
    && unzip /tmp/ninja-linux.zip -d /usr/local/bin \
    && rm /tmp/ninja-linux.zip \
    && wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir ${CMAKE_HOME} \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh

ENV PATH="${CMAKE_HOME}/bin:${PATH}"
