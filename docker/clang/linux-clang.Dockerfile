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
    && echo 'deb http://ppa.launchpad.net/fkrull/deadsnakes/ubuntu precise main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys FF3997E83CD969B409FB24BC5BB92C09DB82666C \
    && apt-get update \
    && apt-get install --no-install-recommends -y gcc-9 g++-9 git zip unzip xz-utils wget ca-certificates make autoconf file patch \
    && apt-get clean \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 9300 --slave /usr/bin/g++ g++ /usr/bin/g++-9

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

ARG BINUTILS_VERSION=2.37
ARG GCC_HOME=/usr

RUN apt-get update && apt-get install --no-install-recommends -y zlib1g-dev && apt-get clean \
    && wget http://ftpmirror.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz -O - | tar -xJ \
    && mkdir binutils-${BINUTILS_VERSION}-linux \
    && cd binutils-${BINUTILS_VERSION}-linux \
    && export CPPFLAGS='-g0 -O2' \
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
    && rm -rf binutils-${BINUTILS_VERSION} \
    && rm -rf binutils-${BINUTILS_VERSION}-linux \
    && apt-get -y purge zlib1g-dev \
    && apt-get -y autoremove \
    && update-alternatives --install /usr/bin/ld ld /usr/bin/x86_64-linux-gnu-ld 10300

ARG LLVM_VERSION=12.0.1

RUN apt-get update \
    && apt-get install --no-install-recommends -y libxml2 zlib1g-dev lzma-dev libxml2-dev libssl-dev python3.5 \
    && apt-get clean \
    && git clone --depth 1 -b llvmorg-${LLVM_VERSION} https://github.com/llvm/llvm-project.git \
    && cd llvm-project \
    && mkdir build \
    && cd build \
    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_SUPPRESS_REGENERATION=ON \
                -DLLVM_TARGETS_TO_BUILD="X86" \
                -DLLVM_ENABLE_PROJECTS="clang;lld" \
                -DLLVM_ENABLE_RUNTIMES="compiler-rt;libcxx;libcxxabi;openmp"\
                -DLLVM_BUILD_LLVM_DYLIB=ON \
                -DLLVM_LINK_LLVM_DYLIB=ON \
                -DCLANG_LINK_CLANG_DYLIB=ON \
                -DCOMPILER_RT_BUILD_SANITIZERS=OFF \
                -DCOMPILER_RT_BUILD_XRAY=OFF \
                -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
                -DCOMPILER_RT_BUILD_PROFILE=OFF \
                -DCOMPILER_RT_BUILD_MEMPROF=OFF \
                -DCOMPILER_RT_INCLUDE_TESTS=OFF \
                -DOPENMP_ENABLE_LIBOMPTARGET_PROFILING=OFF \
                -DLIBOMP_OMPT_SUPPORT=OFF ../llvm \
    && cmake --build . --target install \
    && cd ../../ \
    && rm -rf llvm-project \
    && apt-get purge -y zlib1g-dev lzma-dev libxml2-dev libssl-dev python3.5 \
    && apt-get autoremove -y
