FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /mnt

RUN apt-get update \
    && apt-get install -y wget ca-certificates gnupg2 apt-transport-https \
    && echo 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal main' >> /etc/apt/sources.list \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && apt-get update \
    && apt-get -y install --no-install-recommends llvm clang lld git zip unzip xz-utils make autoconf file patch upx-ucl nsis \
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

ARG MINGW_ROOT=/mingw
ARG GCC_HOME=/usr
ARG BINUTILS_VERSION=2.37
ARG MINGW_VERSION=9.0.0
ARG GCC_VERSION=11.2.0
ARG PKG_CONFIG_VERSION=0.29.2

RUN apt-get update \
    && apt-get -y install --no-install-recommends zlib1g-dev libgmp-dev libmpfr-dev libmpc-dev libssl-dev libisl-dev bzip2 libisl22 libmpc3 \
    && apt-get clean \
    && wget https://pkg-config.freedesktop.org/releases/pkg-config-${PKG_CONFIG_VERSION}.tar.gz -O - | tar -xz \
    && wget https://ftpmirror.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
    && wget https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v${MINGW_VERSION}.tar.bz2/download -O - | tar -xj \
    && wget https://ftpmirror.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -O - | tar -xJ \
    && export CPPFLAGS='-g0' \
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
      --disable-bootstrap \
      --disable-shared \
      --enable-static \
      --enable-threads=posix \
      --disable-multilib \
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
    && rm -rf ${MINGW_ROOT} \
    && apt-get -y purge zlib1g-dev libssl-dev libgmp-dev libmpfr-dev libmpc-dev libisl-dev bzip2 \
    && apt-get -y autoremove \
    && update-alternatives --install ${GCC_HOME}/bin/mingw32-gcc mingw32-gcc ${GCC_HOME}/bin/x86_64-w64-mingw32-gcc 11200 \
        --slave ${GCC_HOME}/bin/mingw32-g++ mingw32-g++ ${GCC_HOME}/bin/x86_64-w64-mingw32-g++ \
    && update-alternatives --install /usr/bin/ld ld ${GCC_HOME}/bin/x86_64-linux-gnu-ld 11200 \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_eh.a \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_s.a
