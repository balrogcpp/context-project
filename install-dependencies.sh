#!/bin/bash

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