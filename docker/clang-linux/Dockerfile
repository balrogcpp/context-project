FROM ubuntu:12.04

ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /mnt

RUN rm /etc/apt/sources.list \
    && echo 'deb http://old-releases.ubuntu.com/ubuntu/ precise main restricted universe multiverse' >> /etc/apt/sources.list \
    && echo 'deb http://old-releases.ubuntu.com/ubuntu/ precise-updates main restricted universe multiverse' >> /etc/apt/sources.list \
    && echo 'deb http://old-releases.ubuntu.com/ubuntu/ precise-security main restricted universe multiverse' >> /etc/apt/sources.list \
    && echo 'deb http://ppa.launchpad.net/git-core/ppa/ubuntu precise main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E1DD270288B4E6030699E45FA1715D88E1DF1F24 \
    && echo 'deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu precise main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 60C317803A41BA51845E371A1E9377A2BA9EF27F \
    && apt-get update \
    && apt-get install -y gcc-9 g++-9 git zip xz-utils wget ca-certificates make autoconf file patch \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 9300 --slave /usr/bin/g++ g++ /usr/bin/g++-9 \
    && apt-get clean



########################################################################################################################
ARG GCC_HOME=/usr
ARG BINUTILS_VERSION=2.37
ARG GCC_VERSION=11.2.0

RUN apt-get install -y zlib1g-dev libgmp-dev libmpfr-dev libmpc-dev libssl-dev libisl-dev \
    && wget http://ftpmirror.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
    && wget http://ftpmirror.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -O - | tar -xJ \
    && mkdir binutils-${BINUTILS_VERSION}-linux \
    && cd binutils-${BINUTILS_VERSION}-linux \
    && ../binutils-${BINUTILS_VERSION}/configure \
      --prefix=${GCC_HOME} \
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
    && mkdir gcc-linux \
    && cd gcc-linux \
    && ../gcc-${GCC_VERSION}/configure \
      --prefix=${GCC_HOME} \
      --target=x86_64-linux-gnu \
      --enable-languages=c,c++ \
      --disable-multilib \
      --enable-libgomp \
      --enable-libatomic \
      --enable-graphite \
      --disable-multilib \
      --enable-lto \
      --disable-werror \
    && make -j`nproc` all-gcc > /dev/null \
    && make install-gcc > /dev/null \
    && cd .. \
    && cd gcc-linux \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && rm -rf binutils-${BINUTILS_VERSION} \
    && rm -rf binutils-${BINUTILS_VERSION}-linux \
    && rm -rf gcc-${GCC_VERSION} \
    && rm -rf gcc-linux \
    && apt-get -y purge zlib1g-dev libssl-dev libgmp-dev libmpfr-dev libmpc-dev libisl-dev \
    && apt-get -y autoremove \
    && apt-get clean

RUN update-alternatives --install ${GCC_HOME}/bin/gcc gcc ${GCC_HOME}/bin/x86_64-linux-gnu-gcc 11200 \
        --slave ${GCC_HOME}/bin/g++ g++ ${GCC_HOME}/bin/x86_64-linux-gnu-g++ \
        && update-alternatives --install /usr/bin/ld ld ${GCC_HOME}/bin/x86_64-linux-gnu-ld 11200

ENV LD_LIBRARY_PATH="${GCC_HOME}/lib64:${LD_LIBRARY_PATH}"



########################################################################################################################
ARG CMAKE_VERSION=3.19.8
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
ARG NINJA_VERSION=1.10.2
ENV PATH="${CMAKE_HOME}/bin:${PATH}"

RUN wget https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip -P /tmp && unzip /tmp/ninja-linux.zip -d /usr/local/bin && rm /tmp/ninja-linux.zip \
    && wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir ${CMAKE_HOME} \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh



########################################################################################################################
ARG PYTHON3_VERSION=3.9.4
ARG PYTHON3_SHORT_VERSION=3.9
ARG LLVM_VERSION=13.0.0
ARG LLVM_HOME=/usr

RUN apt-get update \
    && apt-get install -y libssl-dev zlib1g-dev libncurses5-dev libncursesw5-dev libreadline-dev libsqlite3-dev \
                        libgdbm-dev libbz2-dev libexpat1-dev liblzma-dev libffi-dev \
    && wget https://www.python.org/ftp/python/${PYTHON3_VERSION}/Python-${PYTHON3_VERSION}.tar.xz -O - | tar -xJ \
    && cd Python-${PYTHON3_VERSION} \
    && ./configure --enable-optimization --prefix=/opt/python3 \
    && make -j`nproc` \
    && make install \
    && export PATH=$PATH:/opt/python3/bin \
    && cd ../ \
    && rm -rf Python-${PYTHON3_VERSION} \
    && apt-get purge -y libssl-dev zlib1g-dev libncurses5-dev libncursesw5-dev libreadline-dev libsqlite3-dev \
                             libgdbm-dev libbz2-dev libexpat1-dev liblzma-dev libffi-dev \
    && apt-get autoremove -y \
    && apt-get clean \
    && cd /mnt \
    && apt-get update \
    && apt-get install -y libxml2 zlib1g-dev lzma-dev libxml2-dev libssl-dev \
    && git clone --depth 1 -b llvmorg-${LLVM_VERSION} https://github.com/llvm/llvm-project.git \
    && cd llvm-project \
    && rm -rf .git \
    && mkdir build \
    && cd build \
    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${LLVM_HOME} \
        -DLLVM_TARGETS_TO_BUILD="X86" \
        -DLLVM_ENABLE_PROJECTS=-"llvm;clang;libcxx;libcxxabi;lld;compiler-rt;openmp" ../llvm \
    && cmake --build . --target install \
    && cd ../../ \
    && rm -rf llvm-project \
    && rm -rf /opt/python3 \
    && apt-get purge -y zlib1g-dev lzma-dev libxml2-dev libssl-dev \
    && apt-get autoremove -y \
    && apt-get clean
