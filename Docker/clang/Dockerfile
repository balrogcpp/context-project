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


FROM ubuntu:16.04

ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /mnt

RUN apt-get update \
    && apt-get install -y gcc g++ zip wget ca-certificates gnupg2 make autoconf


# Build GCC&Mingw from source, ubuntu ppa contains old versions
ARG MINGW_ROOT=/mingw
ARG BINUTILS_VERSION=2.35.1
ARG MINGW_VERSION=8.0.0
ARG GCC_VERSION=10.3.0
ARG PKG_CONFIG_VERSION=0.29.2

RUN apt-get install -y xz-utils bzip2 zlib1g-dev libgmp-dev libmpfr-dev libmpc-dev libssl-dev libisl-dev \
    && wget https://pkg-config.freedesktop.org/releases/pkg-config-${PKG_CONFIG_VERSION}.tar.gz -O - | tar -xz \
    && wget https://ftpmirror.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
    && wget https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v${MINGW_VERSION}.tar.bz2/download -O - | tar -xj \
    && wget https://ftpmirror.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -O - | tar -xJ \
    && mkdir -p ${MINGW_ROOT}/include ${MINGW_ROOT}/lib/pkgconfig \
    && cd pkg-config-${PKG_CONFIG_VERSION} \
    && ./configure \
       --prefix=/usr \
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
    && mv /mnt/gcc-mingw32/gcc/xgcc /mnt/gcc-mingw32/gcc/xgcc.bak \
    && echo "#!/bin/bash" >> /mnt/gcc-mingw32/gcc/xgcc \
    && echo "/mnt/gcc-mingw32/gcc/xgcc.bak -Wno-format \$@" >> /mnt/gcc-mingw32/gcc/xgcc \
    && chmod +x /mnt/gcc-mingw32/gcc/xgcc \
    && make -j`nproc` > /dev/null \
    && rm /mnt/gcc-mingw32/gcc/xgcc \
    && mv /mnt/gcc-mingw32/gcc/xgcc.bak /mnt/gcc-mingw32/gcc/xgcc \
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
    && apt-get -y purge zlib1g-dev libssl-dev libgmp-dev libmpfr-dev libmpc-dev libisl-dev \
    && apt-get -y autoremove \
    && apt-get clean \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/x86_64-linux-gnu-gcc 10000 \
        --slave /usr/bin/g++ g++ /usr/bin/x86_64-linux-gnu-g++ \
    && update-alternatives --install /usr/bin/mingw32-gcc mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc 10000 \
        --slave /usr/bin/mingw32-g++ mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++ \
    && ln -s /usr/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a /usr/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_eh.a \
    && ln -s /usr/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a /usr/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_s.a


ENV CMAKE_VERSION=3.20.0
ENV CMAKE_HOME=/opt/cmake
ENV NINJA_VERSION=1.10.2
ENV PATH="${CMAKE_HOME}/bin:${PATH}"

RUN wget https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip -P /tmp && unzip /tmp/ninja-linux.zip -d /usr/local/bin && rm /tmp/ninja-linux.zip \
    && wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir /opt/cmake \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh


ENV LLVM_VERSION=12.0.0-rc5
ENV LLVM_HOME=/usr

RUN apt-get install -y git python3 \
    && cd /mnt \
    && git clone --depth 1 -b llvmorg-${LLVM_VERSION} https://github.com/llvm/llvm-project.git \
    && cd llvm-project \
    && mkdir build \
    && cd build \
    && export LD_LIBRARY_PATH=/usr/lib64 \
    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${LLVM_HOME} -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS=-"llvm;clang;libcxx;libcxxabi;lld;compiler-rt;openmp" ../llvm \
    && cmake --build . --target install \
    && cd ../../ \
    && rm -rf llvm-project \
    && apt-get -y purge git python3 \
    && apt-get -y autoremove \
    && apt-get clean


ENV OSXCROSS_ROOT=/opt/osxcross
ENV OSXCROSS_HOME=/opt
ENV MACOSX_VERSION=10.15
ENV PATH="${OSXCROSS_ROOT}/target/bin:${PATH}"

RUN apt-get install -y git python3 lzma-dev libxml2-dev libssl-dev \
    && cd ${OSXCROSS_HOME} \
    && git clone --depth 1 https://github.com/tpoechtrager/osxcross.git \
    && wget https://github.com/balrogcpp/MacOSXsdk/raw/master/MacOSX${MACOSX_VERSION}.sdk.tar.xz -O ${OSXCROSS_ROOT}/tarballs/MacOSX${MACOSX_VERSION}.sdk.tar.xz \
    && cd ${OSXCROSS_ROOT} \
    && export LD_LIBRARY_PATH=/usr/lib64 \
    && UNATTENDED=1 ./build.sh \
    && rm ${OSXCROSS_ROOT}/tarballs/MacOSX${MACOSX_VERSION}.sdk.tar.xz \
    && rm -rf ${OSXCROSS_ROOT}/build \
    && rm -rf `find  . -type d -name .git` \
    && apt-get -y purge git python3 lzma-dev libxml2-dev libssl-dev \
    && apt-get -y autoremove \
    && apt-get clean
