FROM ubuntu:12.04

ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /mnt


RUN rm /etc/apt/sources.list \
    && echo 'deb http://old-releases.ubuntu.com/ubuntu/ precise main restricted universe multiverse' >> /etc/apt/sources.list \
    && echo 'deb http://old-releases.ubuntu.com/ubuntu/ precise-updates main restricted universe multiverse' >> /etc/apt/sources.list \
    && echo 'deb http://old-releases.ubuntu.com/ubuntu/ precise-security main restricted universe multiverse' >> /etc/apt/sources.list \
    && apt-get update \
    && apt-get install -y zip xz-utils wget ca-certificates gnupg2 make autoconf file patch \
    && apt-get clean



########################################################################################################################
RUN apt-get update \
    && echo 'deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu precise main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 60C317803A41BA51845E371A1E9377A2BA9EF27F \
    && apt-get update \
    && apt-get install -y gcc-9 g++-9 \
    && apt-get clean

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 9300 \
        --slave /usr/bin/g++ g++ /usr/bin/g++-9



########################################################################################################################
ARG PYTHON3_VERSION=3.9.4
ARG PYTHON2_VERSION=2.7.18
ARG PYTHON3_SHORT_VERSION=3.9
ARG PYTHON2_SHORT_VERSION=2.7

RUN apt-get update \
    && apt-get install -y libssl-dev zlib1g-dev libncurses5-dev libncursesw5-dev libreadline-dev libsqlite3-dev \
                        libgdbm-dev libbz2-dev libexpat1-dev liblzma-dev libffi-dev \
    && wget https://www.python.org/ftp/python/${PYTHON3_VERSION}/Python-${PYTHON3_VERSION}.tar.xz -O - | tar -xJ \
    && wget https://www.python.org/ftp/python/${PYTHON2_VERSION}/Python-${PYTHON2_VERSION}.tar.xz -O - | tar -xJ \
    && cd Python-${PYTHON3_VERSION} \
    && ./configure --enable-optimization \
    && make -j`nproc` \
    && make altinstall \
    && cd ../ \
    && rm -rf Python-${PYTHON3_VERSION} \
    && cd Python-${PYTHON2_VERSION} \
    && ./configure --enable-optimization \
    && make -j`nproc` \
    && make altinstall \
    && cd ../ \
    && rm -rf Python-${PYTHON2_VERSION} \
    && apt-get purge -y libssl-dev zlib1g-dev libncurses5-dev libncursesw5-dev libreadline-dev libsqlite3-dev \
                             libgdbm-dev libbz2-dev libexpat1-dev liblzma-dev libffi-dev \
    && apt-get autoremove -y \
    && apt-get clean

RUN update-alternatives --install /usr/local/bin/python3 python3 /usr/local/bin/python${PYTHON3_SHORT_VERSION} 1 \
    && update-alternatives --set python3 /usr/local/bin/python${PYTHON3_SHORT_VERSION} \
    && update-alternatives --install /usr/local/bin/python python /usr/local/bin/python${PYTHON2_SHORT_VERSION} 1 \
    && update-alternatives --set python /usr/local/bin/python${PYTHON2_SHORT_VERSION}



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
ARG BINUTILS_VERSION=2.37

RUN apt-get update \
    && apt-get install -y zlib1g-dev \
    && wget https://ftpmirror.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
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
    && rm -rf binutils-${BINUTILS_VERSION} \
    && rm -rf binutils-${BINUTILS_VERSION}-linux \
    && apt-get -y purge zlib1g-dev \
    && apt-get -y autoremove \
    && apt-get clean

RUN update-alternatives --install /usr/bin/ld ld /usr/bin/x86_64-linux-gnu-ld 10300



########################################################################################################################
RUN apt-get update \
    && echo 'deb http://ppa.launchpad.net/git-core/ppa/ubuntu precise main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E1DD270288B4E6030699E45FA1715D88E1DF1F24 \
    && apt-get update \
    && apt-get install -y git \
    && apt-get clean



########################################################################################################################
ARG LLVM_VERSION=12.0.1
ARG LLVM_HOME=/usr

RUN cd /mnt \
    && apt-get update \
    && apt-get install -y libxml2 zlib1g-dev lzma-dev libxml2-dev libssl-dev \
    && git clone --depth 1 -b llvmorg-${LLVM_VERSION} https://github.com/llvm/llvm-project.git \
    && cd llvm-project \
    && mkdir build \
    && cd build \
    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${LLVM_HOME} \
        -DLLVM_TARGETS_TO_BUILD="X86" \
        -DLLVM_ENABLE_PROJECTS=-"llvm;clang;libcxx;libcxxabi;lld;compiler-rt;openmp" ../llvm \
    && cmake --build . --target install \
    && cd ../../ \
    && rm -rf llvm-project \
    && apt-get purge -y zlib1g-dev lzma-dev libxml2-dev libssl-dev \
    && apt-get autoremove -y \
    && apt-get clean
