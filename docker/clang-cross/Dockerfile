#MIT License
#
#Copyright (c) 2021 Andrew Vasiliev
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.


FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /mnt


RUN apt-get update \
    && apt-get install -y git gcc g++ python python3 zip wget ca-certificates gnupg2 make autoconf file patch \
    && apt-get clean


########################################################################################################################
# Build Mingw from source, ubuntu ppa contains old versions
ARG MINGW_ROOT=/mingw
ARG GCC_HOME=/usr
ARG BINUTILS_VERSION=2.35.1
ARG MINGW_VERSION=8.0.0
ARG GCC_VERSION=10.3.0
ARG PKG_CONFIG_VERSION=0.29.2

RUN apt-get install -y zlib1g-dev libgmp-dev libmpfr-dev libmpc-dev libssl-dev libisl-dev xz-utils bzip2 \
    && wget https://pkg-config.freedesktop.org/releases/pkg-config-${PKG_CONFIG_VERSION}.tar.gz -O - | tar -xz \
    && wget https://ftpmirror.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
    && wget https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v${MINGW_VERSION}.tar.bz2/download -O - | tar -xj \
    && wget https://ftpmirror.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -O - | tar -xJ \
    && mkdir -p ${MINGW_ROOT}/include ${MINGW_ROOT}/lib/pkgconfig \
    && cd pkg-config-${PKG_CONFIG_VERSION} \
    && ./configure \
       --prefix=${GCC_HOME} \
       --with-pc-path=${MINGW_ROOT}/lib/pkgconfig \
       --with-internal-glib \
       --disable-shared \
       --disable-nls \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && mkdir binutils-${BINUTILS_VERSION}-mingw \
    && cd binutils-${BINUTILS_VERSION}-mingw \
    && ../binutils-${BINUTILS_VERSION}/configure \
      --prefix=${GCC_HOME} \
      --target=x86_64-w64-mingw32 \
      --disable-shared \
      --enable-static \
      --enable-lto \
      --enable-plugins \
      --disable-multilib \
      --disable-nls \
      --disable-werror \
      --with-system-zlib \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && mkdir mingw-w64 \
    && cd mingw-w64 \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-headers/configure \
      --prefix=${GCC_HOME}/x86_64-w64-mingw32 \
      --host=x86_64-w64-mingw32 \
      --enable-sdk=all \
      --enable-secure-api \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && mkdir gcc-mingw32 \
    && cd gcc-mingw32 \
    && ../gcc-${GCC_VERSION}/configure \
      --prefix=${GCC_HOME} \
      --target=x86_64-w64-mingw32 \
      --enable-languages=c,c++ \
      --disable-shared \
      --enable-static \
      --enable-threads=posix \
      --enable-exceptions=seh \
      --enable-libgomp \
      --enable-libatomic \
      --enable-graphite \
      --disable-multilib \
      --enable-lto \
      --disable-nls \
      --disable-werror \
    && make -j`nproc` all-gcc > /dev/null \
    && make install-gcc > /dev/null \
    && cd .. \
    && cd mingw-w64 \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-crt/configure \
      --prefix=${GCC_HOME}/x86_64-w64-mingw32 \
      --host=x86_64-w64-mingw32 \
      --enable-wildcard \
      --disable-lib32 \
      --enable-lib64 \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && cd mingw-w64 \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-libraries/winpthreads/configure \
      --prefix=${GCC_HOME}/x86_64-w64-mingw32 \
      --host=x86_64-w64-mingw32 \
      --enable-static \
      --disable-shared \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && cd gcc-mingw32 \
    && mv /mnt/gcc-mingw32/gcc/xgcc /mnt/gcc-mingw32/gcc/xgcc.bak \
    && echo "#!/bin/bash" >> /mnt/gcc-mingw32/gcc/xgcc \
    && echo "/mnt/gcc-mingw32/gcc/xgcc.bak -Wno-format \$@" >> /mnt/gcc-mingw32/gcc/xgcc \
    && chmod +x /mnt/gcc-mingw32/gcc/xgcc \
    && make -j`nproc` > /dev/null \
    && rm /mnt/gcc-mingw32/gcc/xgcc \
    && mv /mnt/gcc-mingw32/gcc/xgcc.bak /mnt/gcc-mingw32/gcc/xgcc \
    && make install > /dev/null \
    && cd .. \
    && rm -rf pkg-config-${PKG_CONFIG_VERSION} \
    && rm -rf binutils-${BINUTILS_VERSION} \
    && rm -rf binutils-${BINUTILS_VERSION}-mingw \
    && rm -rf mingw-w64-v${MINGW_VERSION} \
    && rm -rf mingw-w64 \
    && rm -rf gcc-${GCC_VERSION} \
    && rm -rf gcc-mingw32 \
    && apt-get -y purge zlib1g-dev libssl-dev libgmp-dev libmpfr-dev libmpc-dev libisl-dev xz-utils bzip2 \
    && apt-get -y autoremove \
    && apt-get clean

RUN update-alternatives --install ${GCC_HOME}/bin/mingw32-gcc mingw32-gcc ${GCC_HOME}/bin/x86_64-w64-mingw32-gcc 10300 \
        --slave ${GCC_HOME}/bin/mingw32-g++ mingw32-g++ ${GCC_HOME}/bin/x86_64-w64-mingw32-g++ \
    && update-alternatives --install /usr/bin/ld ld ${GCC_HOME}/bin/x86_64-linux-gnu-ld 10300 \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_eh.a \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_s.a


########################################################################################################################
ENV CMAKE_VERSION=3.20.2
ENV CMAKE_HOME=/opt/cmake
ENV NINJA_VERSION=1.10.2
ENV PATH="${CMAKE_HOME}/bin:${PATH}"

RUN wget https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip -P /tmp && unzip /tmp/ninja-linux.zip -d /usr/local/bin && rm /tmp/ninja-linux.zip \
    && wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir ${CMAKE_HOME} \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh


########################################################################################################################
RUN apt-get install -y wget ca-certificates gnupg2 apt-transport-https \
    && echo 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal main' >> /etc/apt/sources.list \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && apt-get update \
    && apt-get install -y llvm clang lld \
    && apt-get clean


########################################################################################################################
ARG OSXCROSS_ROOT=/opt/osxcross
ARG OSXCROSS_HOME=/opt
ARG MACOSX_VERSION=10.15

RUN apt-get install -y libxml2 lzma-dev libxml2-dev libssl-dev xz-utils \
    && cd ${OSXCROSS_HOME} \
    && git clone --depth 1 https://github.com/tpoechtrager/osxcross.git \
    && wget https://github.com/balrogcpp/MacOSXsdk/raw/master/MacOSX${MACOSX_VERSION}.sdk.tar.xz -O ${OSXCROSS_ROOT}/tarballs/MacOSX${MACOSX_VERSION}.sdk.tar.xz \
    && cd ${OSXCROSS_ROOT} \
    && UNATTENDED=1 ./build.sh \
    && rm ${OSXCROSS_ROOT}/tarballs/MacOSX${MACOSX_VERSION}.sdk.tar.xz \
    && rm -rf ${OSXCROSS_ROOT}/build \
    && rm -rf `find  . -type d -name .git` \
    && apt-get -y purge lzma-dev libxml2-dev libssl-dev xz-utils \
    && apt-get -y autoremove \
    && apt-get clean


RUN eval `${OSXCROSS_ROOT}/target/bin/x86_64-apple-darwin19-osxcross-conf`

ENV OSXCROSS_HOST=x86_64-apple-darwin19
ENV OSXCROSS_TOOLCHAIN_FILE=${OSXCROSS_ROOT}/target/toolchain.cmake
ENV PATH="${OSXCROSS_ROOT}/target/bin:${PATH}"


########################################################################################################################
ENV ANDROID_HOME=/opt/android-sdk

RUN apt-get update \
    && apt-get install -y git wget zip openjdk-8-jdk \
    && apt-get clean

WORKDIR /opt


########################################################################################################################
RUN wget https://dl.google.com/android/repository/commandlinetools-linux-6609375_latest.zip -O tools.zip \
    && unzip tools.zip \
    && rm tools.zip \
    && cd tools/bin \
    && export ANDROID_HOME=/opt/android-sdk \
    && yes | ./sdkmanager  --licenses --sdk_root=${ANDROID_HOME} \
    && cd ../

ENV PATH="/opt/tools/bin:${PATH}"
ENV ANDROID_SDK_ROOT="${ANDROID_HOME}"
