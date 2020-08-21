#MIT License
#
#Copyright (c) 2020 Andrey Vasiliev
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

#Purpose of this image is to provide latest gcc and mingw-w64, only way to get them is manually compile

FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
ENV MINGW=/mingw
ARG BINUTILS_VERSION=2.33.1
ARG MINGW_VERSION=7.0.0
ARG GCC_VERSION=10.2.0
ARG PKG_CONFIG_VERSION=0.29.2
WORKDIR /mnt
ARG CMAKE_VERSION=3.18.1
ARG CMAKE_HOME=/opt/cmake

RUN apt-get update \
    && apt-get install -y g++ gcc make autoconf libssl1.1 wget ca-certificates zlib1g-dev libssl-dev libgmp-dev libmpfr-dev \
        libmpc-dev libisl-dev libgmp10 libmpfr6 libmpc3 yasm file patch gperf xz-utils bzip2 zip \
    && wget https://pkg-config.freedesktop.org/releases/pkg-config-${PKG_CONFIG_VERSION}.tar.gz -O - | tar -xz \
    && wget https://ftpmirror.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
    && wget https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v${MINGW_VERSION}.tar.bz2/download -O - | tar -xj \
    && wget https://ftpmirror.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -O - | tar -xJ \
    && wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
               -O /tmp/cmake-install.sh \
    && mkdir -p ${MINGW}/include ${MINGW}/lib/pkgconfig \
    && cd pkg-config-${PKG_CONFIG_VERSION} \
    && ./configure \
       --prefix=/usr \
       --with-pc-path=${MINGW}/lib/pkgconfig \
       --with-internal-glib \
       --disable-shared \
       --disable-nls \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && mkdir binutils-${BINUTILS_VERSION}-mingw \
    && cd binutils-${BINUTILS_VERSION}-mingw \
    && ../binutils-${BINUTILS_VERSION}/configure \
      --prefix=/usr \
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
    && mkdir binutils-${BINUTILS_VERSION}-linux \
    && cd binutils-${BINUTILS_VERSION}-linux \
    && ../binutils-${BINUTILS_VERSION}/configure \
      --prefix=/usr \
      --target=x86_64-linux-gnu \
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
      --prefix=/usr/x86_64-w64-mingw32 \
      --host=x86_64-w64-mingw32 \
      --enable-sdk=all \
      --enable-secure-api \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && mkdir gcc-mingw32 \
    && cd gcc-mingw32 \
    && ../gcc-${GCC_VERSION}/configure \
      --prefix=/usr \
      --target=x86_64-w64-mingw32 \
      --enable-languages=c,c++ \
      --disable-shared \
      --enable-static \
      --enable-threads=posix \
      --enable-libgomp \
      --enable-libatomic \
      --enable-graphite \
      --enable-libstdcxx-pch \
      --enable-libstdcxx-debug \
      --disable-multilib \
      --enable-lto \
      --disable-nls \
      --disable-werror \
    && make -j`nproc` all-gcc > /dev/null \
    && make install-gcc > /dev/null \
    && cd .. \
    && mkdir gcc-linux \
    && cd gcc-linux \
    && ../gcc-${GCC_VERSION}/configure \
      --prefix=/usr \
      --target=x86_64-linux-gnu \
      --enable-languages=c,c++ \
      --disable-multilib \
    && make -j`nproc` all-gcc > /dev/null \
    && make install-gcc > /dev/null \
    && cd .. \
    && cd mingw-w64 \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-crt/configure \
      --prefix=/usr/x86_64-w64-mingw32 \
      --host=x86_64-w64-mingw32 \
      --enable-wildcard \
      --disable-lib32 \
      --enable-lib64 \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && cd mingw-w64 \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-libraries/winpthreads/configure \
      --prefix=/usr/x86_64-w64-mingw32 \
      --host=x86_64-w64-mingw32 \
      --enable-static \
      --disable-shared \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && cd gcc-mingw32 \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && cd gcc-linux \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && rm -rf pkg-config-${PKG_CONFIG_VERSION} \
    && rm -rf binutils-${BINUTILS_VERSION} \
    && rm -rf binutils-${BINUTILS_VERSION}-mingw \
    && rm -rf binutils-${BINUTILS_VERSION}-linux \
    && rm -rf mingw-w64-v${MINGW_VERSION} \
    && rm -rf mingw-w64 \
    && rm -rf gcc-${GCC_VERSION} \
    && rm -rf gcc-mingw32 \
    && rm -rf gcc-linux \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/x86_64-linux-gnu-gcc 10000 \
        --slave /usr/bin/g++ g++ /usr/bin/x86_64-linux-gnu-g++ \
    && update-alternatives --install /usr/bin/mingw32-gcc mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc 10000 \
        --slave /usr/bin/mingw32-g++ mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++ \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir /opt/cmake \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh

ENV PATH="${CMAKE_HOME}/bin:${PATH}"
