# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM ubuntu:18.04

ARG DEBIAN_FRONTEND=noninteractive
ARG UBUNRU_VERSION=bionic


# git zip etc.
RUN apt-get update \
    && apt-get --no-install-recommends install -y wget ca-certificates gnupg2 apt-transport-https \
    && echo "deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu ${UBUNRU_VERSION} main" >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 60C317803A41BA51845E371A1E9377A2BA9EF27F \
    && apt-get update \
    && apt-get -y install --no-install-recommends git zip unzip xz-utils make autoconf file patch \
    && apt-get clean


# cmake ninja upx
ARG CMAKE_VERSION=3.24.2
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
ARG NINJA_VERSION=1.11.1
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


# gcc and mingw
WORKDIR /mnt
ARG MINGW_ROOT=/mingw
ARG GCC_HOME=/usr
ARG GNU_MIRROR=https://ftp.gnu.org/gnu
## 2.38 is broken
ARG BINUTILS_VERSION=2.37
ARG MINGW_VERSION=10.0.0
# 12.2.0 openal error: undefined symbol: std::__once_callable
ARG GCC_VERSION=11.3.0
ARG PKG_CONFIG_VERSION=0.29.2
ARG GCC_APT_VERSION=10
RUN apt-get update \
    && apt-get -y install --no-install-recommends zlib1g-dev libgmp-dev libmpfr-dev libmpc-dev libssl-dev libisl-dev bzip2 libisl19 libmpc3 \
    && apt-get install --no-install-recommends -y gcc-${GCC_APT_VERSION} g++-${GCC_APT_VERSION} \
    && apt-get clean \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_APT_VERSION} 100 --slave /usr/bin/g++ g++ /usr/bin/g++-${GCC_APT_VERSION} \
    && wget https://pkg-config.freedesktop.org/releases/pkg-config-${PKG_CONFIG_VERSION}.tar.gz -O - | tar -xz \
    && wget ${GNU_MIRROR}/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
    && wget ${GNU_MIRROR}/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -O - | tar -xJ \
    && wget https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v${MINGW_VERSION}.tar.bz2/download -O - | tar -xj \
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
    && make -j`nproc` > /dev/null \
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
    && apt-get -y purge gcc-${GCC_APT_VERSION} g++-${GCC_APT_VERSION} \
    && apt-get -y autoremove \
    && apt-get -y clean \
    && update-alternatives --install ${GCC_HOME}/bin/mingw32-gcc mingw32-gcc ${GCC_HOME}/bin/x86_64-w64-mingw32-gcc 100 \
        --slave ${GCC_HOME}/bin/mingw32-g++ mingw32-g++ ${GCC_HOME}/bin/x86_64-w64-mingw32-g++ \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_eh.a \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_s.a


# clang; libgcc-7 is for ubuntu 18.04, newer ubuntus have newer libgcc
ARG CLANG_VERSION=15
RUN echo "deb http://apt.llvm.org/${UBUNRU_VERSION} llvm-toolchain-${UBUNRU_VERSION}-${CLANG_VERSION} main" >> /etc/apt/sources.list \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && apt-get update \
    && apt-get -y install --no-install-recommends llvm-${CLANG_VERSION} clang-${CLANG_VERSION} lld-${CLANG_VERSION} libgcc-7-dev libstdc++-7-dev \
    && apt-get clean

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${CLANG_VERSION} 100 --slave /usr/bin/clang++ clang++ /usr/bin/clang++-${CLANG_VERSION} \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/clang-${CLANG_VERSION} 100 --slave /usr/bin/c++ c++ /usr/bin/clang++-${CLANG_VERSION} \
    && update-alternatives --install /usr/bin/lld lld /usr/bin/lld-${CLANG_VERSION} 100 \
    && update-alternatives --install /usr/bin/llvm-ar llvm-ar /usr/bin/llvm-ar-${CLANG_VERSION} 100

ENV PATH="/usr/lib/llvm-${CLANG_VERSION}/bin:$PATH"


# apple
WORKDIR /mnt
ARG OSXCROSS_ROOT=/opt/osxcross
ARG MACOS_SDK_VERSION=12.3
ARG MACOS_SDK_CODE=21.4
RUN apt-get update \
    && apt-get install --no-install-recommends -y libxml2 lzma-dev libxml2-dev libssl-dev python \
    && apt-get clean \
    && git clone https://github.com/tpoechtrager/osxcross.git \
    && wget https://github.com/balrogcpp/MacOSXsdk/raw/master/MacOSX${MACOS_SDK_VERSION}.sdk.tar.xz -O ./osxcross/tarballs/MacOSX${MACOS_SDK_VERSION}.sdk.tar.xz \
    && cd osxcross \
    && git checkout 50e86ebca7d14372febd0af8cd098705049161b9 \
    && rm -rf .git \
    && UNATTENDED=1 TARGET_DIR=${OSXCROSS_ROOT} ./build.sh \
    && cd .. \
    && rm -rf osxcross \
    && apt-get -y purge lzma-dev libxml2-dev libssl-dev python \
    && apt-get -y autoremove
ENV OSXCROSS_HOST=x86_64-apple-darwin${MACOS_SDK_CODE}
ENV OSXCROSS_HOST_X86_64=x86_64-apple-darwin${MACOS_SDK_CODE}
ENV OSXCROSS_HOST_ARM64=aarch64-apple-darwin${MACOS_SDK_CODE}
ENV OSXCROSS_TOOLCHAIN_FILE=${OSXCROSS_ROOT}/toolchain.cmake
ENV PATH="${OSXCROSS_ROOT}/bin:${PATH}"
ENV X86_64_EVAL=`x86_64-apple-darwin${MACOS_SDK_CODE}-osxcross-conf`
ENV ARM64_EVAL=`arm64-apple-darwin${MACOS_SDK_CODE}-osxcross-conf`
