FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
ARG UBUNTU_VERSION=focal
WORKDIR /var/build


# git zip etc.
RUN apt-get update \
    && apt-get -y install --no-install-recommends curl ca-certificates gnupg2 apt-transport-https software-properties-common \
    && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && apt-get update \
    && apt-get -y install --no-install-recommends zip unzip xz-utils make autoconf file patch \
    && apt-get -y purge software-properties-common \
    && apt-get -y autoremove --purge \
    && apt-get clean


ARG CLANG_MAJOR=21
ARG CLANG_VERSION=2100
ARG STDLIB_APT_MAJOR=15
RUN echo "deb http://apt.llvm.org/${UBUNTU_VERSION} llvm-toolchain-${UBUNTU_VERSION}-${CLANG_MAJOR} main" >> /etc/apt/sources.list \
    && curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && apt-get update \
    && apt-get -y install --no-install-recommends llvm-${CLANG_MAJOR} clang-${CLANG_MAJOR} lld-${CLANG_MAJOR} libgcc-${STDLIB_APT_MAJOR}-dev libstdc++-${STDLIB_APT_MAJOR}-dev \
    && sed -i '$d' /etc/apt/sources.list \
    && apt-get update \
    && apt-get clean \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${CLANG_MAJOR} ${CLANG_VERSION} --slave /usr/bin/clang++ clang++ /usr/bin/clang++-${CLANG_MAJOR} \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/clang-${CLANG_MAJOR} ${CLANG_VERSION} --slave /usr/bin/c++ c++ /usr/bin/clang++-${CLANG_MAJOR} \
    && update-alternatives --install /usr/bin/lld lld /usr/bin/lld-${CLANG_MAJOR} ${CLANG_VERSION} \
    && update-alternatives --install /usr/bin/llvm-ar llvm-ar /usr/bin/llvm-ar-${CLANG_MAJOR} ${CLANG_VERSION} \
    && update-alternatives --install /usr/bin/llvm-nm llvm-nm /usr/bin/llvm-nm-${CLANG_MAJOR} ${CLANG_VERSION} \
    && update-alternatives --install /usr/bin/llvm-strip llvm-strip /usr/bin/llvm-strip-${CLANG_MAJOR} ${CLANG_VERSION} \
    && update-alternatives --install /usr/bin/llvm-config llvm-config /usr/bin/llvm-config-${CLANG_MAJOR} ${CLANG_VERSION} \
    && update-alternatives --install /usr/bin/llvm-ranlib llvm-ranlib /usr/bin/llvm-ranlib-${CLANG_MAJOR} ${CLANG_VERSION}
ENV PATH="/usr/lib/llvm-${CLANG_MAJOR}/bin:$PATH"


# gcc and mingw
ARG MINGW_ROOT=/mingw
ARG GCC_HOME=/usr
ARG GNU_MIRROR=https://sourceware.org/pub
ARG FREEDESKTOP_MIRROR=https://pkg-config.freedesktop.org
ARG BINUTILS_VERSION=2.46.0
ARG MINGW_VERSION=14.0.0
ARG GCC_VERSION=15.2.0
ARG GCC_VERSION_DEC=1520
ARG PKG_CONFIG_VERSION=0.29.2
ARG GCC_APT_MAJOR=13
ARG GCC_APT_MAJOR_DEC=1300
RUN apt-get update \
    && curl -fSL ${FREEDESKTOP_MIRROR}/releases/pkg-config-${PKG_CONFIG_VERSION}.tar.gz | tar -xz \
    && curl -fSL ${GNU_MIRROR}/binutils/releases/binutils-${BINUTILS_VERSION}.tar.xz | tar -xJ \
    && curl -fSL ${GNU_MIRROR}/gcc/releases/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz | tar -xJ \
    && curl -fSL https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v${MINGW_VERSION}.tar.bz2/download  | tar -xj \
    && apt-get -y install --no-install-recommends zlib1g-dev libgmp-dev libmpfr-dev libmpc-dev libssl-dev libisl-dev bzip2 libisl22 libmpc3 \
    && apt-get -y install --no-install-recommends gcc-${GCC_APT_MAJOR} g++-${GCC_APT_MAJOR} texinfo \
    && apt-get clean \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_APT_MAJOR} ${GCC_APT_MAJOR_DEC} --slave /usr/bin/g++ g++ /usr/bin/g++-${GCC_APT_MAJOR} \
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
    && apt-get -y purge gcc-${GCC_APT_MAJOR} g++-${GCC_APT_MAJOR} texinfo \
    && apt-get -y autoremove --purge \
    && apt-get -y clean \
    && update-alternatives --install ${GCC_HOME}/bin/mingw32-gcc mingw32-gcc ${GCC_HOME}/bin/x86_64-w64-mingw32-gcc ${GCC_VERSION_DEC} \
        --slave ${GCC_HOME}/bin/mingw32-g++ mingw32-g++ ${GCC_HOME}/bin/x86_64-w64-mingw32-g++ \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_eh.a \
    && ln -s ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc.a ${GCC_HOME}/lib/gcc/x86_64-w64-mingw32/${GCC_VERSION}/libgcc_s.a


# apple
COPY ./tarballs ./tarballs
ARG OSXCROSS_ROOT=/usr/local/osxcross
ARG OSXCROSS_GIT_HASH=e6ab3fa7423f9235ce9ed6381d6d3af191b46b59
ARG CMAKE_VERSION=4.3.3
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
RUN apt-get update \
    && apt-get -y install --no-install-recommends git python3 libxml2 lzma-dev libxml2-dev libssl-dev libbz2-dev \
    && curl -fsSL -O https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && mkdir ${CMAKE_HOME} && sh cmake-${CMAKE_VERSION}-Linux-x86_64.sh --skip-license --prefix=${CMAKE_HOME} && rm -f cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && export PATH="${CMAKE_HOME}/bin:${PATH}" \
    && curl -fsSL https://github.com/tpoechtrager/osxcross/archive/${OSXCROSS_GIT_HASH}.zip -o osxcross.zip \
    && unzip osxcross.zip && rm -f osxcross.zip && mv osxcross-${OSXCROSS_GIT_HASH} osxcross \
    && cd osxcross \
    && cp -r ../tarballs . && rm -rf ../tarballs \
    && UNATTENDED=1 TARGET_DIR=${OSXCROSS_ROOT} bash build.sh \
    && rm -rf ../osxcross \
    && rm -rf ${CMAKE_HOME} \
    && apt-get -y purge git python3 lzma-dev libxml2-dev libssl-dev libbz2-dev \
    && apt-get -y autoremove --purge \
    && apt-get clean
ENV PATH="${OSXCROSS_ROOT}/bin:${PATH}"
ENV OSXCROSS_TOOLCHAIN_FILE=${OSXCROSS_ROOT}/toolchain.cmake
