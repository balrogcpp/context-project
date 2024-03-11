FROM ubuntu:18.04

ARG DEBIAN_FRONTEND=noninteractive
ARG UBUNTU_VERSION=bionic
WORKDIR /var/build


# git zip etc.
RUN apt-get update \
    && apt-get -y install --no-install-recommends wget curl ca-certificates gnupg2 apt-transport-https software-properties-common \
    && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && apt-get update \
    && apt-get -y install --no-install-recommends zip unzip xz-utils make autoconf file patch \
    && apt-get -y purge software-properties-common \
    && apt-get -y autoremove \
    && apt-get clean


# gcc and mingw
ARG MINGW_ROOT=/mingw
ARG GCC_HOME=/usr
ARG GNU_MIRROR=https://ftp.gnu.org/gnu
# 2.38;2.39;2.40 are broken
ARG BINUTILS_VERSION=2.37
ARG MINGW_VERSION=11.0.1
ARG GCC_VERSION=13.2.0
ARG PKG_CONFIG_VERSION=0.29.2
ARG GCC_APT_MAJOR=11
RUN apt-get update \
    && apt-get -y install --no-install-recommends zlib1g-dev libgmp-dev libmpfr-dev libmpc-dev libssl-dev libisl-dev bzip2 libisl19 libmpc3 \
    && apt-get -y install --no-install-recommends gcc-${GCC_APT_MAJOR} g++-${GCC_APT_MAJOR} \
    && apt-get clean \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_APT_MAJOR} 100 --slave /usr/bin/g++ g++ /usr/bin/g++-${GCC_APT_MAJOR} \
    && wget https://pkg-config.freedesktop.org/releases/pkg-config-${PKG_CONFIG_VERSION}.tar.gz -q -O - | tar -xz \
    && wget ${GNU_MIRROR}/binutils/binutils-${BINUTILS_VERSION}.tar.xz -q -O - | tar -xJ \
    && wget ${GNU_MIRROR}/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -q -O - | tar -xJ \
    && wget https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v${MINGW_VERSION}.tar.bz2/download -q -O - | tar -xj \
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
      --enable-gprofng=no \
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
    && apt-get -y purge gcc-${GCC_APT_MAJOR} g++-${GCC_APT_MAJOR} \
    && apt-get -y autoremove \
    && apt-get -y clean \
    && update-alternatives --install ${GCC_HOME}/bin/mingw32-gcc mingw32-gcc ${GCC_HOME}/bin/x86_64-w64-mingw32-gcc 1000 \
        --slave ${GCC_HOME}/bin/mingw32-g++ mingw32-g++ ${GCC_HOME}/bin/x86_64-w64-mingw32-g++ \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_eh.a \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_s.a


# clang; libgcc-7 is for ubuntu 18.04, newer ubuntus have newer libgcc
ARG CLANG_MAJOR=18
ARG STDLIB_APT_MAJOR=11
RUN echo "deb http://apt.llvm.org/${UBUNTU_VERSION} llvm-toolchain-${UBUNTU_VERSION}-${CLANG_MAJOR} main" >> /etc/apt/sources.list \
    && wget -q -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && apt-get update \
    && apt-get -y install --no-install-recommends llvm-${CLANG_MAJOR} clang-${CLANG_MAJOR} lld-${CLANG_MAJOR} libgcc-${STDLIB_APT_MAJOR}-dev libstdc++-${STDLIB_APT_MAJOR}-dev \
    && sed -i '$d' /etc/apt/sources.list \
    && apt-get update \
    && apt-get clean \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${CLANG_MAJOR} 1000 --slave /usr/bin/clang++ clang++ /usr/bin/clang++-${CLANG_MAJOR} \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/clang-${CLANG_MAJOR} 1000 --slave /usr/bin/c++ c++ /usr/bin/clang++-${CLANG_MAJOR} \
    && update-alternatives --install /usr/bin/lld lld /usr/bin/lld-${CLANG_MAJOR} 1000 \
    && update-alternatives --install /usr/bin/llvm-ar llvm-ar /usr/bin/llvm-ar-${CLANG_MAJOR} 1000
ENV PATH="/usr/lib/llvm-${CLANG_MAJOR}/bin:$PATH"


# apple
COPY ./tarballs ./tarballs
ARG OSXCROSS_ROOT=/opt/osxcross
ARG OSXCROSS_GIT_HASH=ff8d100f3f026b4ffbe4ce96d8aac4ce06f1278b
ARG CMAKE_VERSION=3.28.3
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
RUN apt-get update \
    && apt-get -y install --no-install-recommends git python libxml2 lzma-dev libxml2-dev libssl-dev libbz2-dev \
    && wget -q https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && mkdir ${CMAKE_HOME} && sh cmake-${CMAKE_VERSION}-Linux-x86_64.sh --skip-license --prefix=${CMAKE_HOME} && rm cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && export PATH="${CMAKE_HOME}/bin:${PATH}" \
    && apt-get clean \
    && git clone https://github.com/tpoechtrager/osxcross.git \
    && cd osxcross \
    && git checkout ${OSXCROSS_GIT_HASH} \
    && git apply ../tarballs/osxcross.patch \
    && cp -r ../tarballs . && rm -rf ../tarballs \
    && UNATTENDED=1 TARGET_DIR=${OSXCROSS_ROOT} bash build.sh \
    && rm -rf ../osxcross \
    && rm -rf ${CMAKE_HOME} \
    && apt-get -y purge git python lzma-dev libxml2-dev libssl-dev libbz2-dev \
    && apt-get -y autoremove \
    && apt-get clean
ENV PATH="${OSXCROSS_ROOT}/bin:${PATH}"
ENV OSXCROSS_TOOLCHAIN_FILE=${OSXCROSS_ROOT}/toolchain.cmake
