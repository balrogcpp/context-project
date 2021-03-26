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

FROM balrogcpp/xio-dependencies:latest

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

ADD sources ./sources
ADD deploy ./deploy
ADD CMake ./CMake
ADD demo ./demo
ADD doc ./doc
ADD tests ./tests
ADD LICENSE .
ADD android ./android
ADD programs ./programs
ADD assets ./assets
ADD CMakeLists.txt .
ADD zip-dependencies.py .
ADD dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt

ENV CC=clang
ENV CXX=clang++

RUN mkdir -p build-windows build-linux build-android \
    && python3 zip-dependencies.py \
    && cd build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-w64.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target package \
    && cd .. \
    && cd build-linux \
    && cmake -DCMAKE_BUILD_TYPE=Release -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target package \
    && cd ..
