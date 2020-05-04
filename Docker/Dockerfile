FROM ubuntu:18.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update &&\
    apt-get install software-properties-common -y &&\
    add-apt-repository ppa:ubuntu-toolchain-r/test -y &&\
    apt-get update > /dev/null &&\
    apt-get install -y \
        g++-9 \
        git \
        subversion \
        mercurial \
        libssl1.1 \
        wget \
        ca-certificates \
        ninja-build \
        doxygen \
        doxygen-latex \
        graphviz \
        libxaw7-dev \
        libxrandr-dev \
        libglew-dev \
        libpulse-dev \
        > /dev/null

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 900 --slave /usr/bin/g++ g++ /usr/bin/g++-9

RUN apt-get install -y \
    zlib1g-dev \
    libssl-dev \
    libgmp-dev \
    libmpfr-dev \
    libmpc-dev \
    libisl-dev \
    libgmp10 \
    libmpfr6 \
    libmpc3 \
    libisl19 \
    yasm \
    file \
    patch \
    gperf \
    xz-utils \
    bzip2 \
    zip \
    nsis

ENV MINGW=/mingw

ARG CMAKE_VERSION=3.16.4
ARG BINUTILS_VERSION=2.33.1
ARG MINGW_VERSION=7.0.0
ARG GCC_VERSION=9.2.0
ARG PKG_CONFIG_VERSION=0.29.2

WORKDIR /mnt

RUN wget -q https://pkg-config.freedesktop.org/releases/pkg-config-${PKG_CONFIG_VERSION}.tar.gz -O - | tar -xz
RUN wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ
RUN wget -q https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -O - | tar -xJ
RUN git clone -b v7.0.0 --depth 1 https://github.com/mirror/mingw-w64.git mingw-w64-v${MINGW_VERSION}
RUN mkdir -p ${MINGW}/include ${MINGW}/lib/pkgconfig

RUN cd pkg-config-${PKG_CONFIG_VERSION} \
    && ./configure \
       --prefix=/usr \
       --with-pc-path=${MINGW}/lib/pkgconfig \
       --with-internal-glib \
       --disable-shared \
       --disable-nls \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd ..

RUN cd binutils-${BINUTILS_VERSION} \
    && ./configure \
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
    && cd ..

RUN mkdir mingw-w64 \
    && cd mingw-w64 \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-headers/configure \
      --prefix=/usr/x86_64-w64-mingw32 \
      --host=x86_64-w64-mingw32 \
      --enable-sdk=all \
      --enable-secure-api \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd ..

RUN mkdir gcc \
    && cd gcc \
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
    && cd ..

RUN cd mingw-w64 \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-crt/configure \
      --prefix=/usr/x86_64-w64-mingw32 \
      --host=x86_64-w64-mingw32 \
      --enable-wildcard \
      --disable-lib32 \
      --enable-lib64 \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd ..

RUN cd mingw-w64 \
    && ../mingw-w64-v${MINGW_VERSION}/mingw-w64-libraries/winpthreads/configure \
      --prefix=/usr/x86_64-w64-mingw32 \
      --host=x86_64-w64-mingw32 \
      --enable-static \
      --disable-shared \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd ..

RUN cd gcc \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd ..

RUN wget https://github.com/Kitware/CMake/releases/download/v3.15.5/cmake-3.15.5-Linux-x86_64.sh \
         -q -O /tmp/cmake-install.sh \
         && chmod u+x /tmp/cmake-install.sh \
         && mkdir /usr/bin/cmake \
         && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
         && rm /tmp/cmake-install.sh

RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
         -q -O /tmp/cmake-install.sh \
         && chmod u+x /tmp/cmake-install.sh \
         && mkdir /usr/bin/cmake \
         && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
         && rm /tmp/cmake-install.sh

ENV PATH="/usr/bin/cmake/bin:${PATH}"
