#!/bin/bash
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

sudo apt-get update &&\
sudo apt-get install -y \
        g++ \
        gcc \
        g++-10 \
        gcc-10 \
        make \
        checkinstall \
        git \
        autoconf \
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
        zlib1g-dev \
        libssl-dev \
        libgmp-dev \
        libmpfr-dev \
        libmpc-dev \
        libisl-dev \
        libgmp10 \
        libmpfr6 \
        libmpc3 \
        yasm \
        file \
        patch \
        gperf \
        xz-utils \
        bzip2 \
        zip \
        nsis

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10000 --slave /usr/bin/g++ g++ /usr/bin/g++-10

exit 0
