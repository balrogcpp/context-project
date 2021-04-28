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


FROM balrogcpp/gcc-mingw:latest

ARG DEBIAN_FRONTEND=noninteractive
WORKDIR /mnt

# To guarantee latest git is used
RUN apt-get update \
    && echo 'deb http://ppa.launchpad.net/git-core/ppa/ubuntu bionic main' >> /etc/apt/sources.list \
    && apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E1DD270288B4E6030699E45FA1715D88E1DF1F24 \
    && apt-get update \
    && apt-get install -y git


########################################################################################################################
ARG PYTHON3_VERSION=3.9.4
ARG PYTHON2_VERSION=2.7.18
ARG PYTHON3_SHORT_VERSION=3.9
ARG PYTHON2_SHORT_VERSION=2.7

RUN apt-get install -y libssl-dev zlib1g-dev libncurses5-dev libncursesw5-dev libreadline-dev libsqlite3-dev \
                        libgdbm-dev libbz2-dev libexpat1-dev liblzma-dev libffi-dev \
    && wget https://www.python.org/ftp/python/${PYTHON3_VERSION}/Python-${PYTHON3_VERSION}.tar.xz -O - | tar -xJ\
    && wget https://www.python.org/ftp/python/${PYTHON2_VERSION}/Python-${PYTHON2_VERSION}.tar.xz -O - | tar -xJ\
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
ARG LLVM_VERSION=12.0.0
ARG LLVM_HOME=/usr

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


########################################################################################################################
ARG OSXCROSS_ROOT=/opt/osxcross
ARG OSXCROSS_HOME=/opt
ARG MACOSX_VERSION=10.15

RUN apt-get install -y lzma-dev libxml2-dev libssl-dev \
    && cd ${OSXCROSS_HOME} \
    && git clone --depth 1 https://github.com/tpoechtrager/osxcross.git \
    && wget https://github.com/balrogcpp/MacOSXsdk/raw/master/MacOSX${MACOSX_VERSION}.sdk.tar.xz -O ${OSXCROSS_ROOT}/tarballs/MacOSX${MACOSX_VERSION}.sdk.tar.xz \
    && cd ${OSXCROSS_ROOT} \
    && UNATTENDED=1 ./build.sh \
    && rm ${OSXCROSS_ROOT}/tarballs/MacOSX${MACOSX_VERSION}.sdk.tar.xz \
    && rm -rf ${OSXCROSS_ROOT}/build \
    && rm -rf `find  . -type d -name .git` \
    && apt-get -y purge lzma-dev libxml2-dev libssl-dev \
    && apt-get -y autoremove \
    && apt-get clean

RUN eval `${OSXCROSS_ROOT}/target/bin/x86_64-apple-darwin19-osxcross-conf`

ENV OSXCROSS_HOST=x86_64-apple-darwin19
ENV OSXCROSS_TOOLCHAIN_FILE=${OSXCROSS_ROOT}/target/toolchain.cmake
ENV PATH="${OSXCROSS_ROOT}/target/bin:${PATH}"
