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


FROM ubuntu:14.04

ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /mnt

RUN apt-get update \
    && apt-get install -y zip wget ca-certificates gnupg2 make autoconf file patch xz-utils bzip2

########################################################################################################################
RUN apt-get update \
    && echo 'deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu trusty main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 60C317803A41BA51845E371A1E9377A2BA9EF27F \
    && apt-get update \
    && apt-get install -y gcc-9 g++-9



# To guarantee latest git is used
RUN apt-get update \
    && echo 'deb http://ppa.launchpad.net/git-core/ppa/ubuntu trusty main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E1DD270288B4E6030699E45FA1715D88E1DF1F24 \
    && apt-get update \
    && apt-get install -y git


########################################################################################################################
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



########################################################################################################################
RUN apt-get update \
    && echo 'deb http://ppa.launchpad.net/deadsnakes/ppa/ubuntu trusty main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys F23C5A6CF475977595C89F51BA6932366A755776 \
    && apt-get update \
    && apt-get install -y python2.7 python3.7



########################################################################################################################
ARG LLVM_VERSION=12.0.0
ARG LLVM_HOME=/usr

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 10300 \
        --slave /usr/bin/g++ g++ /usr/bin/g++-9

RUN cd /mnt \
    && apt-get install -y libxml2 zlib1g-dev lzma-dev libxml2-dev libssl-dev \
    && git clone --depth 1 -b llvmorg-${LLVM_VERSION} https://github.com/llvm/llvm-project.git \
    && cd llvm-project \
    && mkdir build \
    && cd build \
    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${LLVM_HOME} \
        -DLLVM_ENABLE_PROJECTS=-"llvm;clang;libcxx;libcxxabi;lld;compiler-rt;openmp" ../llvm \
    && cmake --build . --target install \
    && cd ../../ \
    && rm -rf llvm-project \
    && apt-get purge -y zlib1g-dev lzma-dev libxml2-dev libssl-dev \
    && apt-get autoremove -y \
    && apt-get clean
