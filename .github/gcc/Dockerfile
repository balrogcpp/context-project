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
    && apt-get install --no-install-recommends -y git zip unzip xz-utils wget ca-certificates make autoconf file patch \
    && apt-get clean


ARG GCC_HOME=/usr
ARG GNU_MIRROR=https://ftp.gnu.org/gnu
## 2.38;2.39;2.40 is broken
ARG BINUTILS_VERSION=2.37
# 12.2.0 mingw openal error: undefined symbol: std::__once_callable
ARG GCC_VERSION=13.1.0
ARG GCC_APT_MAJOR=9
RUN apt-get update \
    && apt-get -y install --no-install-recommends zlib1g-dev libssl-dev libgmp-dev libmpfr-dev libmpc-dev libisl-dev libisl10 libmpc2 \
    && apt-get -y install --no-install-recommends gcc-${GCC_APT_MAJOR} g++-${GCC_APT_MAJOR} \
    && apt-get clean \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_APT_MAJOR} 100 --slave /usr/bin/g++ g++ /usr/bin/g++-${GCC_APT_MAJOR} \
    && wget http://ftpmirror.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
    && wget http://ftpmirror.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -O - | tar -xJ \
    && export CPPFLAGS='-g0' \
    && mkdir binutils-${BINUTILS_VERSION}-linux \
    && cd binutils-${BINUTILS_VERSION}-linux \
    && ../binutils-${BINUTILS_VERSION}/configure \
      --prefix=${GCC_HOME} \
      --target=x86_64-linux-gnu \
      --disable-multilib \
      --disable-nls \
      --disable-werror \
      --with-system-zlib \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && cd .. \
    && mkdir gcc-linux \
    && cd gcc-linux \
    &&  ../gcc-${GCC_VERSION}/configure \
      --prefix=${GCC_HOME} \
      --target=x86_64-linux-gnu \
      --enable-languages=c,c++ \
      --disable-multilib \
      --disable-nls \
      --disable-werror \
      --disable-bootstrap \
    && make -j`nproc` all-gcc > /dev/null \
    && make install-gcc > /dev/null \
    && cd .. \
    && cd gcc-linux \
    && make -j`nproc` > /dev/null \
    && make install > /dev/null \
    && export CPPFLAGS='' \
    && cd .. \
    && rm -rf binutils-${BINUTILS_VERSION} \
    && rm -rf binutils-${BINUTILS_VERSION}-linux \
    && rm -rf gcc-${GCC_VERSION} \
    && rm -rf gcc-linux \
    && apt-get -y purge zlib1g-dev libssl-dev libgmp-dev libmpfr-dev libmpc-dev libisl-dev \
    && apt-get -y autoremove \
    && update-alternatives --install /usr/bin/gcc gcc ${GCC_HOME}/bin/x86_64-linux-gnu-gcc 100 \
        --slave /usr/bin/g++ g++ ${GCC_HOME}/bin/x86_64-linux-gnu-g++ \
        --slave /usr/bin/ar ar ${GCC_HOME}/bin/x86_64-linux-gnu-ar \
        --slave /usr/bin/ld ld ${GCC_HOME}/bin/x86_64-linux-gnu-ld \
    && export PATH="${GCC_HOME}/bin:${PATH}" \
    && export LD_LIBRARY_PATH="${GCC_HOME}/lib64:${LD_LIBRARY_PATH}"

ENV PATH="${GCC_HOME}/bin:${PATH}"
ENV LD_LIBRARY_PATH="${GCC_HOME}/lib64:${LD_LIBRARY_PATH}"


# cmake ninja upx
ARG CMAKE_VERSION=3.26.3
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
ARG NINJA_VERSION=1.11.1
ARG UPX_VERSION=4.0.2
RUN cd /tmp \
    && wget -q https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip \
    && unzip -q /tmp/ninja-linux.zip -d /usr/local/bin && rm ninja-linux.zip \
    && wget -q https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && mkdir ${CMAKE_HOME} && sh cmake-${CMAKE_VERSION}-Linux-x86_64.sh --skip-license --prefix=${CMAKE_HOME} && rm cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    && wget https://github.com/upx/upx/releases/download/v${UPX_VERSION}/upx-${UPX_VERSION}-amd64_linux.tar.xz -q  -O - | tar -xJ \
    && mv upx-${UPX_VERSION}-amd64_linux/upx /usr/local/bin && rm -rf upx-${UPX_VERSION}-amd64_linux
ENV PATH="${CMAKE_HOME}/bin:${PATH}"


#ARG LLVM_VERSION=12.0.1
#ARG BINUTILS_VERSION=2.37
#ARG BINUTILS_HOME=/usr
#ARG PYTHON3_VERSION=3.9.9
#ARG PYTHON3_SHORT_VERSION=3.9
#ARG PYTHON3_HOME=/opt/python-${PYTHON3_SHORT_VERSION}
#RUN apt-get update \
#    && apt-get install --no-install-recommends -y gcc-9 g++-9 zlib1g-dev libisl10 libmpc2 \
#    && apt-get clean \
#    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 9300 --slave /usr/bin/g++ g++ /usr/bin/g++-9 \
#    && wget http://ftpmirror.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
#    && mkdir binutils-${BINUTILS_VERSION}-linux \
#    && cd binutils-${BINUTILS_VERSION}-linux \
#    && export CPPFLAGS='-g0' \
#    && ../binutils-${BINUTILS_VERSION}/configure \
#      --prefix=${BINUTILS_HOME} \
#      --disable-multilib \
#      --disable-nls \
#      --disable-werror \
#      --with-system-zlib \
#    && make -j`nproc` > /dev/null \
#    && make install > /dev/null \
#    && export CPPFLAGS='' \
#    && cd .. \
#    && rm -rf binutils-${BINUTILS_VERSION} \
#    && rm -rf binutils-${BINUTILS_VERSION}-linux \
#    && apt-get install --no-install-recommends -y libssl-dev zlib1g-dev libncurses5-dev libncursesw5-dev \
#                       libreadline-dev libsqlite3-dev libgdbm-dev libbz2-dev libexpat1-dev liblzma-dev libffi-dev \
#    && apt-get clean \
#    && wget https://www.python.org/ftp/python/${PYTHON3_VERSION}/Python-${PYTHON3_VERSION}.tar.xz -O - | tar -xJ \
#    && cd Python-${PYTHON3_VERSION} \
#    && ./configure --prefix=${PYTHON3_HOME} \
#    && make -j`nproc` \
#    && make altinstall \
#    && cd ../ \
#    && rm -rf Python-${PYTHON3_VERSION} \
#    && apt-get purge -y libssl-dev zlib1g-dev libncurses5-dev libncursesw5-dev libreadline-dev libsqlite3-dev \
#                             libgdbm-dev libbz2-dev libexpat1-dev liblzma-dev libffi-dev \
#    && apt-get autoremove -y \
#    && update-alternatives --install /usr/local/bin/python3 python3 ${PYTHON3_HOME}/bin/python${PYTHON3_SHORT_VERSION} 3990 \
#    && apt-get install --no-install-recommends -y zlib1g-dev lzma-dev libxml2-dev libssl-dev \
#    && apt-get clean \
#    && git clone --depth 1 -b llvmorg-${LLVM_VERSION} https://github.com/llvm/llvm-project.git \
#    && cd llvm-project \
#    && mkdir build \
#    && cd build \
#    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release \
#                -DCMAKE_SUPPRESS_REGENERATION=ON \
#                -DLLVM_TARGETS_TO_BUILD="X86" \
#                -DLLVM_ENABLE_PROJECTS="clang;lld" \
#                -DLLVM_ENABLE_RUNTIMES="compiler-rt;libcxx;libcxxabi;openmp" \
#                -DLLVM_BUILD_LLVM_DYLIB=ON \
#                -DLLVM_LINK_LLVM_DYLIB=ON \
#                -DCLANG_LINK_CLANG_DYLIB=ON \
#                -DCOMPILER_RT_BUILD_SANITIZERS=OFF \
#                -DCOMPILER_RT_BUILD_XRAY=OFF \
#                -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
#                -DCOMPILER_RT_BUILD_PROFILE=OFF \
#                -DCOMPILER_RT_BUILD_MEMPROF=OFF \
#                -DCOMPILER_RT_INCLUDE_TESTS=OFF \
#                -DOPENMP_ENABLE_LIBOMPTARGET_PROFILING=OFF \
#                -DLIBOMP_OMPT_SUPPORT=OFF ../llvm \
#    && cmake --build . --target install \
#    && cd ../../ \
#    && rm -rf llvm-project \
#    && apt-get purge -y gcc-9 g++-9 zlib1g-dev lzma-dev libxml2-dev libssl-dev \
#    && apt-get autoremove -y \
#    && rm -rf ${PYTHON3_HOME} \
#    && apt-get install --no-install-recommends -y libgcc-9-dev libstdc++-9-dev \
#    && apt-get clean
